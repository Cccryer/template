using namespace std;
#include<atomic>
template <typename T>
class shared_ptr{

public:
    //默认构造
    shared_ptr(): ptr(nullptr), count(nullptr){}

    //构造
    explicit shared_ptr(T* _ptr): ptr(_ptr), count(nullptr){
        if(ptr){
            count = new atomic<int>(1);
        }
    }

    //拷贝构造
    shared_ptr(const shared_ptr<T>& other): ptr(other.ptr), count(other.count){
        if(count){
            count->fetch_add(1, memory_order_relaxed);
        }
    }

    //移动构造
    shared_ptr(shared_ptr<T>&& other) noexcept : ptr(other.ptr), count(other.count){
        other.ptr = nullptr;
        other.count = nullptr;
    }

    //拷贝赋值
    shared_ptr<T>& operator=(const shared_ptr<T>& other){
        if(this != &other){
            release();

            ptr = other.ptr;
            count = other.count;
            count->fetch_add(1, memory_order_relaxed)
        }
        return *this;
    }

    //移动赋值
    shared_ptr<T>& operator=(shared_ptr<T>&& other){
        if(this != &other){
            release();
            ptr = other.ptr;
            count = other.count;
            other.ptr = nullptr;
            other.count = nullptr;
        }
        return *this;
    }

    //析构
    ~shared_ptr(){
        release();
    }

    //解引用
    T& operator*() const {
        return *ptr;
    }

    //指针
    T* operator->() const{
        return ptr;
    }


    T* get() const{
        return ptr;
    }

    //reset
    void reset(){
        release();
        ptr = nullptr;
        count = nullptr;
    }

    void reset(T* _ptr){
        release();
        ptr = _ptr;
        count = ptr? new atomic<int>(1) : nullptr;
    }

    //make_shared
    template<typename... Args>
    static shared_ptr<T> make_shared(Args&&... args){
        
        T* obj = new T(forward<Args>(args)...);
        atomic<int>* t_count = new atomic<int>(1);

        shared_ptr<T> sp;
        sp.ptr = obj;
        sp.count = count;
        return sp;
    }


private:
    void release(){
        if(count != nullptr && count->fetch_sub(1) == 1){
            delete ptr;
            delete count;
        }
        ptr = nullptr;
        count = nullptr;
    }

private:
    T *ptr;
    atomic<int> *count;
};