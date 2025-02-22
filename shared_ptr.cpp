using namespace std;
#include<atomic>
template <typename T>
class shared_ptr{

public:
    //默认构造
    shared_ptr(): ptr(nullptr), count(new atomic<int>(0)){}

    //构造
    explicit shared_ptr(T* _ptr): ptr(_ptr), count(new atomic<int>(1)){}

    //拷贝构造
    shared_ptr(const shared_ptr<T>& other): ptr(other.ptr), count(other.count){
        ++(*count);
    }

    //拷贝赋值
    shared_ptr<T>& operator=(const shared_ptr<T>& other){
        if(this != &other){
            release();

            ptr = other.ptr;
            count = other.count;
            ++(*count);
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
        count = new atomic<int>(0); //这里要重新初始化count，不然还是和其它指针指向相同地址;
    }

    void reset(T* _ptr){
        release();
        ptr = _ptr;
        count = new atomic<int>(_ptr == nullptr ? 0 : 1);
    }

    //make_shared
    static shared_ptr<T> make_shared(T* _ptr){ //注意make_shared为statc
        shared_ptr<T> sp(_ptr);
        return sp;
    }

private:
    void release(){
        if(count != nullptr && --(*count) == 0){
            delete ptr;
            delete count;
        }
        ptr = nullptr;
        count = nullptr;
    }

private:
    T *ptr;
    atmoic<int> *count;
};