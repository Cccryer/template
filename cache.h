#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include <list>
#include <vector>
#include <mutex>
#include <thread>
#include <time.h>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <chrono>

template <typename K, typename V>
class TTLEvictMultiCache {
 private:
    struct CacheValue {
        std::vector<V> values;
        int64_t expired_time;
        bool has_read{false}; // only get can set true
        bool is_clean{true};  // only set can set false
    };
    using CacheItem = std::pair<K, CacheValue>;
    using ItemIteator = typename std::list<CacheItem>::iterator;

 public:
    explicit TTLEvictMultiCache(std::size_t capacity, int64_t ttl_seconds)
        : capacity_(capacity), ttl_(ttl_seconds), stop_(false), clear_signal_(false),
        hot_list_size_(0), cold_list_size_(capacity) {
            cold_list_.resize(capacity_);
            map_.reserve(capacity_);

            // start a clear thread in backguard
            clear_thread_ = std::thread([this]() {
                for(;;) {
                    std::unique_lock<std::mutex> lock(this->mu_);
                    this->cond_.wait(lock, [this](){
                        this->stop_.load() || this->clear_signal_.load();
                    });

                    if (stop_.load()) {return;}
                    {
                        std::lock_guard<std::mutex> lock(this->cold_list_mu_);
                        this->_ClearClodList();
                    }
                    clear_signal_.store(false);
                }
            });
        }
    ~TTLEvictMultiCache() {
        {
            std::lock_guard<std::mutex> lock(mu_);
            stop_.store(true);
        }
        cond_.notify_all();
        clear_thread_.join();
    }

    std::size_t Set(const K& key, V&& value) {
        int64_t now = static_cast<int64_t>(time(nullptr));
        // lock hot_list
        std::lock_guard<std::mutex> lock(hot_list_mu_);
        const auto& it = map_.find(key);
        if (it != map_.end()) {
            auto list_it = it->second;
            list_it->second.values.push_back(std::move(value));
            list_it->second.expired_time = now + ttl_;
            list_it->second.has_read = false;
            hot_list_.splice(hot_list_.begin(), hot_list_, list_it);
            return list_it->second.values.size();
        }
        
        if (cold_list_size_ <= 0) {
            // need clear hot_list
            _BackConditionEvict(now);
        }

        // move cold head to use
        {
            std::lock_guard<std::mutex> lock(cold_list_mu_);
            hot_list_.splice(hot_list_.begin(), cold_list_, cold_list_.begin());
            ++hot_list_size_;
            --cold_list_size_;
        }
        map_[key] = hot_list_.begin();
        CacheItem& node = hot_list_.front();
        node.first = std::move(key);
        node.second.expired_time = now + ttl_;
        node.second.is_clean = false;
        node.second.has_read = false;
        std::vector<V> values{std::move(value)};
        std::swap(node.second.values, values);
        // let we notify clear thread at last, try avoid contention cold_list_mu with clear thread
        if (clear_signal_.load()) {cond_.notify_one();}
        return 1;
    }

    std::vector<V> Get(const K& key) {
        std::lock_guard<std::mutex> lock(hot_list_mu_);
        const auto& it = map_.find(key);
        if (it == map_.end())
            return {};

        auto& list_it = it->second;
        list_it->second.has_read = true;
        hot_list_.splice(hot_list_.end(), hot_list_, list_it);

        return list_it->second.values;
    }

 private:
    //  must call this func with lock hot_list.
    //  this func will lock cold_list_mu.
    //  if success move node to cold list will set clear_signal_ true.
    //  else only expand
    //  will update hot_list_num and cold_list_num
    void _BackConditionEvict(int64_t now) {
        std::size_t moved = 0;
        auto rit = hot_list_.rbegin();
        // clear has been read node
        while(rit != hot_list_.rend() && rit->second.has_read){
            map_.erase(rit->first);
            ++rit;
            ++moved;
        }
        // clear expired node
        if (rit == hot_list_.rbegin()) {
            while (rit != hot_list_.rend() && rit->second.expired_time <= now){
                map_.erase(rit->first);
                ++rit;
                ++moved;
            }
        }

        if (rit == hot_list_.rbegin()) {
            // too small, we must expand capacity
            {
                std::lock_guard<std::mutex> lock(cold_list_mu_);
                cold_list_.resize(capacity_);
            }
            capacity_ *= 2;
            map_.reserve(capacity_);
            cold_list_size_ += capacity_;
        } else {
            // move node to cold_list
            std::lock_guard<std::mutex> lock(cold_list_mu_);
            if (rit == hot_list_.rend()) {
                cold_list_.splice(cold_list_.end(), hot_list_.begin(), hot_list_.end());
            } else {
                cold_list_.splice(cold_list_.end(), rit.base(), hot_list_.end());
            }
            hot_list_size_ -= moved;
            cold_list_size_ += moved;
            clear_signal_.store(true);
        }

        return;
    }

    // must call this func with lock cold_list_mu
    void _ClearClodList() {
        // back begin clear, until is clean
        auto rit = cold_list_.rbegin();
        while(rit != cold_list_.rend() && !rit->second.is_clean) {
            (*rit).swap(CacheItem{});
            ++rit;
        }
    }

 private:
    std::list<CacheItem> hot_list_;
    std::size_t hot_list_size_;
    std::mutex hot_list_mu_;
    std::list<CacheItem> cold_list_;
    std::size_t cold_list_size_;
    std::mutex cold_list_mu_;
    std::unordered_map<K, ItemIteator> map_;
    std::size_t capacity_;
    int64_t ttl_;

    std::thread clear_thread_;
    std::mutex mu_;
    std::condition_variable cond_;
    std::atomic<bool> clear_signal_;
    std::atomic<bool> stop_;
};


// use as base bucket, no ttl and capacity, cant del, only clear
template <typename K, typename V>
class BaseMultiCache {
 public:
    std::size_t Set(const K& key, V&& value) {
        std::lock_guard<std::mutex> lock(mu_);
        const auto& it = map_.find(key);
        if (it == map_.end()) {
            map_[key] = std::vector<V>{std::move(value)};
            return 1;
        }
        it->second.push_back(std::move(value));
        return it->second.size();
    }

    std::vector<V> Get(const K& key) {
        std::lock_guard<std::mutex> lock(mu_);
        const auto& it = map_.find(key);
        if (it == map_.end()) {
            return {};
        }
        return it->second;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mu_);
        map_.clear();
    }

 private:
    std::mutex mu_;
    std::unordered_map<K, std::vector<V>> map_;
};

template <typename K, typename V>
class TTLRollingMultiCache {

 public:

    explicit TTLRollingMultiCache(std::size_t capacity, int64_t ttl_seconds)
        : capacity_(capacity), ttl_(ttl_seconds), start_tick_(_NowSeconds()), cur_(0), clean_signal_(false), stop_(false) {
        buckets_.resize(ttl_seconds+1);
        last_tick_ = start_tick_;

        clear_thread_ = std::thread([this]() {
            for(;;) {
                std::unique_lock<std::mutex> lock(mu_);
                cond_.wait(lock, [this]() { return stop_.load(std::memory_order_acquire) || clean_signal_.load(std::memory_order_acquire); });
                if (stop_.load(std::memory_order_acquire)) break;
                std::size_t expired_index = (cur_.load(std::memory_order_acquire) + 1) % (ttl_ + 1);
                buckets_[expired_index].clear();
                clean_signal_.store(false, std::memory_order_release);
            }
        });
    }

    ~TTLRollingMultiCache() {
        {
            std::lock_guard<std::mutex> lock(mu_);
            stop_.store(true, std::memory_order_release);
        }
        cond_.notify_all();
        clear_thread_.join();
    }

    TTLRollingMultiCache(const TTLRollingMultiCache&) = delete;
    TTLRollingMultiCache& operator=(const TTLRollingMultiCache&) = delete;

    std::size_t Set(const K& key, V&& value) {
        _TickIfNeeded();
        return buckets_[cur_.load(std::memory_order_acquire)].Set(key, std::move(value));
    }

    std::vector<V> Get(const K& key) {
        std::vector<V> res;
        for(const auto& bucket_ : buckets_) {
            res = bucket_.Get(key);
            if (!res.empty())
                break;
        }
        return res;
    }

 private:
    inline int64_t _NowSeconds() {
        return std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::steady_clock::now().time_since_epoch())
            .count();
    }

    void _TickIfNeeded() {
        
        int64_t now = _NowSeconds();
        int64_t old = last_tick_.load(std::memory_order_acquire);
        int64_t elapsed = now - old;
        if (elapsed <= 0) return;
        // in multi thread, now will been update to latest now
        while (now > old && !last_tick_.compare_exchange_weak(old, now,
                 std::memory_order_release, std::memory_order_relaxed) ) {}

        // only last_tick thread can update cur, so cur also will been update to latest
        if (now == last_tick_.load(std::memory_order_acquire)) {
            cur_.store((now - start_tick_) % (ttl_ + 1), std::memory_order_release);
            clean_signal_.store(true, std::memory_order_release);
            cond_.notify_one();
        }
    }

 private:
    std::vector<BaseMultiCache<K, V>> buckets_;
    std::size_t capacity_;
    int64_t ttl_;
    std::atomic<std::size_t> cur_;
    const int64_t start_tick_;
    std::atomic<int64_t> last_tick_;
    std::thread clear_thread_;
    std::atomic<bool> stop_;
    std::atomic<bool> clean_signal_;
    std::condition_variable cond_;
    std::mutex mu_;

};
