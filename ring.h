#ifndef RING_H
#define RING_H

#include<atomic>
#include<memory>
#include<string>
#include<cstring>
#include<iostream>

#define CACHE_LINE_SIZE 128
#define RELAXED std::memory_order_relaxed
#define ACQUIRE std::memory_order_acquire
#define RELEASE std::memory_order_release
#define SEQCST  std::memory_order_seq_cst
#define ACQREL  std::memory_order_acq_rel

# define likely(x)  __builtin_expect(!!(x), 1)
# define unlikely(x) __builtin_expect(!!(x), 0)

template<class T>
class Ring{

public:
    
    explicit Ring(const char* name, uint32_t count)
        : capacity_(count)
    {

        if(name != nullptr){
            strncpy(name_, name, sizeof(name_));
        }

        size_ = ceilToPowerOfTwo(count);
        mask_ = size_ - 1;

        prod_head_.store(0, RELAXED);
        prod_tail_.store(0, RELAXED);
        con_head_.store(0, RELAXED);
        con_tail_.store(0, RELAXED);

        buffer_ = std::make_unique<T*[]>(size_);
    }

    ~Ring() = default;

    inline uint32_t enqueue_bulk(T* const *obj_table, uint32_t n,  uint32_t* free_space){
        uint32_t free_entries;
        uint32_t prod_head;
        uint32_t prod_next;
        uint32_t max = n;
        bool success;

        //预定位置
        do{
            n = max;

            prod_head = this->prod_head_.load(RELAXED);

            free_entries = capacity_ + this->con_tail_.load(ACQUIRE) - prod_head;

            if(unlikely(n > free_entries)){
                n = free_entries;
            }

            if(n == 0)
                return 0;
            
            prod_next = prod_head + n;

            success = this->prod_head_.compare_exchange_weak(prod_head, prod_next, RELEASE, RELAXED);

        }while(unlikely(!success));

        //填入元素
        uint32_t idx = prod_head & this->mask_;
        uint32_t i;
        if(likely(idx + n <= this->size_)){
            for( i = 0; i < n; ++i){
                this->buffer_[idx + i] = obj_table[i];
            }
        }else{
            for ( i = 0; idx < this->size_; i++, idx++){
                this->buffer_[idx] = obj_table[i];
            }
            for (idx = 0; i < n; i++, idx++){
                this->buffer_[idx] = obj_table[i];
            }
            
        }

        while(this->prod_tail_.load(RELAXED) != prod_head);
        this->prod_tail_.store(prod_next, RELEASE);
        // while(!this->prod_tail_.compare_exchange_weak(prod_head, prod_next, RELEASE, RELAXED)); //这里用CAS是错的，CAS失败时会将prod_head更新，但prod_head不应该更新

        if(free_space != nullptr)
            *free_space = free_entries - n;
        return n;
    }

    inline uint32_t dequeue_bulk(T* *const obj_table, uint32_t n, uint32_t* availbile){
        uint32_t con_head;
        uint32_t con_next;
        uint32_t prod_tail;
        uint32_t max = n;
        uint32_t entries;

        bool success;

        
        do{
            n = max;
            
            con_head = this->con_head_.load(RELAXED); //其实compare_exchange_weak失败时会更新con_head为r->con_head,但gcc4.8以前有bug，故在这补救

            prod_tail = this->prod_tail_.load(ACQUIRE);

            entries = prod_tail - con_head; //溢出循环

            if(n > entries)
                n = entries;
            if(unlikely(n == 0))
                return 0;

            con_next = con_head + n;

            success = this->con_head_.compare_exchange_weak(con_head, con_next, RELEASE, RELAXED);
        }while(unlikely(!success));

        uint32_t idx = con_head & this->mask_;
        uint32_t i;

        if(likely(idx + n <= this->size_)){
            for(i = 0; i < n; ++i){
                obj_table[i] = this->buffer_[idx + i];
            }
        }else{
            for(i = 0; idx < this->size_; ++i, ++idx){
                obj_table[i] = this->buffer_[idx];
            }

            for(idx = 0; i < n; ++i, ++idx){
                obj_table[i] = this->buffer_[idx];
            }
        }

        while(this->con_tail_.load(RELAXED) != con_head);
        this->con_tail_.store(con_next, RELEASE);

        // while(!this->con_tail_.compare_exchange_weak(con_head, con_next, RELEASE, RELAXED));//这里用CAS是错的，CAS失败时会将con_head更新，但con_head不应该更新

        if(availbile != nullptr){
            *availbile = entries - n;
        }
        return n;
    }

    inline void show_data(){
        uint32_t prod_tail = this->prod_tail_.load(RELAXED);
        uint32_t con_head = this->con_head_.load(RELAXED);
        if (con_head < prod_tail){
            for(uint32_t i = con_head; i < prod_tail; ++i){
                std::cout << "Element: " << *this->buffer_[i] << std::endl;
            }
        }
        else{
            for(uint32_t i = con_head; i < this->size_; ++i){
                std::cout << "Element: " << *this->buffer_[i] << std::endl;
            }
            for(uint32_t i = 0; i < prod_tail; ++i){
                std::cout << "Element: " << *this->buffer_[i] << std::endl;
            }
        }
    }

    inline uint32_t data_size(){
        uint32_t prod_tail = this->prod_tail_.load(RELAXED);
        uint32_t con_head = this->con_head_.load(RELAXED);
        return (prod_tail - con_head) & this->mask_;
    }

    inline uint32_t capacity(){
        return this->capacity_;
    }

    inline uint32_t size(){
        return this->size_;
    }


private:
    inline uint32_t ceilToPowerOfTwo(uint32_t n){
        if (n == 0)
            return 1;
        n--;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        return n+1;
    }


private:
    alignas(CACHE_LINE_SIZE) char name_[64];
    uint32_t size_;
    uint32_t capacity_;
    uint32_t mask_;

    alignas(CACHE_LINE_SIZE) std::atomic<uint32_t> prod_head_;
    alignas(CACHE_LINE_SIZE) std::atomic<uint32_t> prod_tail_;

    alignas(CACHE_LINE_SIZE) std::atomic<uint32_t> con_head_;
    alignas(CACHE_LINE_SIZE) std::atomic<uint32_t> con_tail_;

    std::unique_ptr<T*[]> buffer_;

};


#endif