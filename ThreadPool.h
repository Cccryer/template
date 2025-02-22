#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<decltype(f(args...))>;
    ~ThreadPool();
private:
    //线程
    std::vector< std::thread > workers;
    //task
    std::queue< std::function<void()> > tasks;
    
    //同步
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};
 
// the constructor just launches some amount of workers

inline
ThreadPool::ThreadPool(size_t threads): stop(false){
    for(size_t i = 0; i < threads; ++i){
        workers.emplace_back(
            [this](){
                for(;;){
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        this->condition.wait(lock, [this](){
                            return this->stop || !this->tasks.empty(); //这里为false才阻塞
                        });
                        if(this->stop && this->tasks.empty())
                            return;
                        
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            }
        );
    }
}

// 提交task
template<typename F, typename... Args> 
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))> //万能引用，函数返回值推断后置
{
    //f的返回值类型
    using f_return_type = decltype(f(args...));

    std::function<f_return_type()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...); //完美转发

    auto task = std::make_shared< std::packaged_task<f_return_type()>>(func);

    std::future<f_return_type> res = task->get_future();

    {
        //限定作用域，提前释放锁
        std::unique_lock<std::mutex> lock(queue_mutex);

        tasks.emplace([task](){
            (*task)();
        });

        condition.notify_one();
    }

    return res;
}


// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}

#endif