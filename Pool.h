#include<queue>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<thread>
#include<future>
using namespace std;

class Pool{

    Pool(size_t );
    
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
        ->future<decltype(f(args...))>;

    ~Pool();
    
private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex mu;
    condition_variable cond;
    bool stop;
};

inline
Pool::Pool(size_t threads): stop(false){
    for(size_t i = 0; i < threads; ++i){
        workers.emplace_back(
            [this](){
                for(;;){
                    function<void()> task;
                    
                    {
                        unique_lock<mutex> lock(mu);
                        this->cond.wait(lock, [this](){
                            return this->stop || !this->tasks.empty();
                        });
                        
                        if(this->stop && this->tasks.empty())
                            return;
                        task = move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            }
        );
    }
}

template<typename F, typename... Args>
auto Pool::enqueue(F&& f, Args&&... args) -> future<decltype(f(args...))>
{
    using f_return_type = decltype(f(args...));
    //万能引用 + 完美转发
    function<f_return_type()> func = bind(forward<F>(f), forward<Args>(args)...);

    auto task = make_shared<packaged_task<f_return_type()>>(func);

    future<f_return_type> res = task->get_future();

    {
        unique_lock<mutex> lock(mu);
        tasks.emplace([task](){
            (*task)();
        });

        cond.notify_one();
    }
    return res;
}

inline
Pool::~Pool(){
    {
        unique_lock<mutex> lock(mu);
        stop = true;
    }
    cond.notify_all();
    for(thread &worker : workers){
        worker.join();
    }

}