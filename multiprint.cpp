#include<thread>
#include<mutex>
#include<condition_variable>
#include<atomic>
#include<iostream>


void print_by_mutex(int num){
    using namespace std;
    
    mutex m_mutex;
    int cnt = 0;

    auto printnum = [&m_mutex, &cnt, &num](int idx, int n){
        while(cnt < num){
            unique_lock<mutex> lock(m_mutex);
            if(cnt > num)
                return;
            if(cnt % n == idx){
                cout<<'('<<idx<<')'<<cnt<<endl;
                ++cnt;
            }
        }
    };

    thread t1 = thread(printnum, 0, 2);
    thread t2 = thread(printnum, 1, 2);

    t1.join();
    t2.join();

}

void print_by_cond(int num){
    using namespace std;

    condition_variable condition;
    mutex m_mutex;
    int cnt = 0;

    auto printnum = [&condition, &m_mutex, &num, &cnt](int idx, int n){
        
        while(cnt < num){
            unique_lock<mutex> lock(m_mutex);

            condition.wait(lock, [&](){
                return cnt % n == idx;
            });

            if(cnt > num)
                return; //防止多打

            cout << '(' << idx << ')' << cnt << endl;
            ++cnt;
            condition.notify_all();
        }

    };

    thread t1 = thread(printnum, 0, 3);
    thread t2 = thread(printnum, 1, 3);
    thread t3 = thread(printnum, 2, 3);

    t1.join();
    t2.join();
    t3.join();

}


void print_by_atomic(int num){
    
    using namespace std;
    atomic<int> cnt(0);

    auto print_num = [&cnt, &num](int idx, int n){
        
        while(cnt.load(memory_order_acquire) <= num){
            int _cnt = cnt.load(memory_order_acquire);
            if(_cnt > num)
                return;
            if(_cnt % n == idx){
                cout << '(' << idx << ')' << cnt.load(memory_order_relaxed) << endl;
                cnt.fetch_add(1, memory_order_release);
            }
        }

    };

    thread t1 = thread(print_num, 0, 3);
    thread t2 = thread(print_num, 1, 3);
    thread t3 = thread(print_num, 2, 3);

    t1.join();
    t2.join();
    t3.join();
}

int main(){
    // print_by_cond(100);
    // print_by_mutex(100);
    print_by_atomic(100);
}

