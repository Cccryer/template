#include<queue>
#include<mutex>
#include<memory>
template <typename T>
class SafeQueue{

public:

    void enqueue(T &t){
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(t);
    }

    bool dequeue(T &t){
        std::unique_lock<std::mutex> lock(m_mutex);
        
        if(!m_queue.empty()){
            t = std::move(m_queue.front()); //注意move，需要T有移动赋值
            m_queue.pop();
            return true;
        }
        return false;
    }

    bool empty(){
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    bool size(){
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }


private:
    std::queue<T> m_queue;
    std::mutex m_mutex;

};