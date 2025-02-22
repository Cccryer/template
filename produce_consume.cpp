#include<queue>
#include<mutex>
#include<condition_variable>
#include<atomic>
#include<thread>



int main(){
    using namespace std;
    int sz = 101;
    vector<int> buffer(sz, 0); 
    int head = 0, tail = 0; 
    mutex m_mutex;
    condition_variable not_full, not_empty;

    auto producer = [&](int idx ){
        for(;;){
            {
                unique_lock<mutex> lock(m_mutex);
                
                not_full.wait(lock, [&](){
                    return (tail + 1) % sz != head; //不满
                });

                printf("PRODUCER[%d]: buffer[%d]: %d -> %d\n", idx, tail, buffer[tail], 1);
                
                buffer[tail] = 1;
                tail = (tail + 1) % sz;
            }
            //唤醒前先释放锁,效率更高
            not_empty.notify_one();
        }
    };

    auto consumer = [&](int idx){
        for(;;){
            {
                unique_lock<mutex> lock(m_mutex);

                not_empty.wait(lock, [&](){
                    return tail != head; //不空
                });

                printf("CONSUMER[%d]: buffer[%d]: %d -> %d\n", idx, head, buffer[head], 0);

                buffer[head] = 0;
                head = (head + 1) % sz;
            }

            not_full.notify_one();
        }
    };

    thread p1(producer, 0);
    thread p2(producer, 1);
    thread p3(producer, 2);
    
    thread s1(consumer, 0);
    thread s2(consumer, 1);
    thread s3(consumer, 2);

    p1.join();
    p2.join();
    p3.join();

    s1.join();
    s2.join();
    s3.join();

}