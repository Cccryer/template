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
}

