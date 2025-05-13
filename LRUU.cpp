#include <unordered_map>
#include <ctime>
using namespace std;

class LRUU{

public:

    LRUU(size_t _capacity): capacity(_capacity), size(0), head(nullptr), end(nullptr) {}
    
    void put(int key, int val, time_t expire_time){
        time_t cur_time = time(nullptr);
        if(mp.find(key) != mp.end()){
            ListNode *node = mp[key];
            node->val = val;
            node->expire_time = cur_time + expire_time;
            moveHead(node);
        }else{
            if(size >= capacity){
                ListNode *p = end;
                while(p->pre != nullptr){
                    if(p->expire_time <= cur_time)
                        break;
                    p = p->pre;
                }

                if(p->expire_time <= cur_time){
                    removeNode(p);
                }else{
                    removeNode(end);
                }
            }
            ListNode *node = new ListNode(key, val, cur_time + expire_time);
            mp[key] = node;
            node->next = head;
            if(head = nullptr){
                head = end = node;
            }else{
                head->pre = node;
                head = node;
            }
            ++size;
        }
    }

    int get(int key){
        if(mp.find(key) == mp.end()){
            return -1;
        }
        ListNode *node = mp[key];
        time_t cur_time = time(nullptr);
        if(node->expire_time <= cur_time){
            removeNode(node);
            return -1;
        }
        moveHead(node);
        return node->val;
    }
private:
    
    struct ListNode
    {
        int key;
        int val;
        time_t expire_time;
        ListNode *pre;
        ListNode *next;

        ListNode(int _key, int _val): key(_key), val(_val), next(nullptr), pre(nullptr), expire_time(0){}
        ListNode(int _key, int _val, time_t _expire_time): key(_key), val(_val), expire_time(_expire_time), next(nullptr), pre(nullptr){}
    };
    
private:
    void moveHead(ListNode *node){
        if(node == nullptr)
            return;
        if(node != head){
            if(node != end){
                node->pre->next = node->next;
                node->next->pre = node->pre;
            }else{
                node->pre->next = nullptr;
                end = node->pre;
            }
            head->pre = node;
            node->next = head;
            node->pre = nullptr;
            head = node;
        }
    }

    void moveTail(ListNode *node){
        if(node == nullptr)
            return;
        
        if(node != end){
            if(node != head){
                node->pre->next = node->next;
                node->next->pre = node->pre;
            }else{
                node->next->pre = nullptr;
                head = node->next;
            }
            
            node->next = nullptr;
            node->pre = end;
            end->next = node;
            end = node;
        }
    }

    void removeNode(ListNode *node){
        if(node == nullptr)
            return;
        mp.erase(node->key);
        --size;
        if(node == head && node == end){
            head = end = nullptr;
            delete node;
            return;
        }
        if(node == head){
            node->next->pre = nullptr;
            head = node->next;
            delete node;
            return;
        }
        if(node == end){
            node->pre->next = nullptr;
            end = node->pre;
            delete node;
            return;
        }

        node->pre->next = node->next;
        node->next->pre = node->pre;

        delete node;
        return;
    }

private:
    size_t size;
    size_t capacity;
    ListNode *head;
    ListNode *end;
    unordered_map<int, ListNode*> mp;

};