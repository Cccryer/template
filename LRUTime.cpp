#include<vector>
#include<ctime>
#include<unordered_map>
using namespace std;
class LRU{
//达到过期时间应该立即被删除
public:
    LRU(int _capacity): capacity(_capacity), size(0), head(nullptr), end(nullptr) {}

    int get(int key){
        if(mp.find(key) == mp.end()){
            return -1;
        }
        ListNode *node = mp[key];
        time_t cur_time = time(nullptr);
        if(node->expire_time <= cur_time){
            //已过期
            removeNode(node);
            return -1;
        }
        moveToHead(node);
        return node->val;
    }

    void put(int key, int val, int expire_seconds){
        time_t cur_time = time(nullptr);
        auto it = mp.find(key);
        if(it != mp.end()){
            ListNode *node = it->second;
            
            //不管过不过期都重设时间
            node->val = val;
            node->expire_time = cur_time + expire_seconds;
            moveToHead(node);
        }else{
            if(size >= capacity){
                ListNode *p = end;
                while(p->prev != nullptr){
                    if(p->expire_time <= cur_time)
                        break;
                    p = p->prev;
                }
                if(p->expire_time <= cur_time){
                    removeNode(p);
                }else{
                    //没有过期的，则清除最久未使用的
                    removeNode(end);
                }
            }

            ListNode *node = new ListNode(key, val, cur_time+expire_seconds);
            mp[key]  = node;
            node->next = head;
            if(head == nullptr){
                head = end = node;
            }else{
                head->prev = node;
                head = node;
            }
            ++size;
        }

    }


private:

    struct ListNode{
        int key;
        int val;
        time_t expire_time;
        ListNode *next;
        ListNode *prev;
        
        ListNode(int _key, int _val, time_t _expire_time): key(_key), val(_val), expire_time(_expire_time), next(nullptr), prev(nullptr){}
    };

    void moveToHead(ListNode *node){
        if(node == nullptr || node == head){
            return;
        }

        if(node != end){
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }else{
            node->prev->next = nullptr;
            end  = node->prev;
        }
        node->next = head;
        head->prev = node;
        node->prev = nullptr;
        head = node;
    }

    void moveToTail(ListNode *node){
        if(node == nullptr || node == end){
            return;
        }

        if(node != head){
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }else{
            node->next->prev = nullptr;
            head = node->next;
        }

        node->prev = end;
        node->next = nullptr;
        end->next = node;
        end = node;
    }

    void removeNode(ListNode *node){
        if(node == nullptr)
            return;
        if(node == end){
            if(node->prev){
                node->prev->next = nullptr;
                end = node->prev;
            }else{
                head = end = nullptr;
            }
        }

        if(node == head){
            if(node->next){
                node->next->prev = nullptr;
                head = node->next;
            }else{
                head = end = nullptr;
            }
        }

        mp.erase(node->key);
        delete node;
        --size;
    }

private:
    size_t size;
    size_t capacity;
    ListNode *head;
    ListNode *end;
    unordered_map<int, ListNode*> mp;
};