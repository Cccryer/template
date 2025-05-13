#include<memory>
#include<iostream>
using namespace std;
//shared_ptr 循环引用不释放的例子
class B;

class A{
public:
    shared_ptr<B> ptr_b;
    ~A(){
        cout<<"A is deleted"<<endl;
    }
};

class B{
public:
    shared_ptr<A> ptr_a;

    ~B(){
        cout<<"B is deleted"<<endl;
    }
};

int main(){
    shared_ptr<A> a(new A);
    shared_ptr<B> b(new B);

    a->ptr_b = b;
    b->ptr_a = a;

    //在栈帧退出时，a对象由于还在被b引用所以无法析构，其内部ptr_b也不会减少
    //b对象由于a中ptr_b还在也无法析构
}

