#include<vector>
using namespace std;


template < typename K, typename V>
class HashMap{

public:
    HashMap(size_t _capacity = 10) : capacity(_capacity), size(0){
        table.resize(_capacity, nullptr);
    } 

    void insert(const K& key, const V& val){
        size_t idx = hash(key);
        Node *node = table[idx];

        while(node != nullptr){
            //防hash碰撞
            if(node->key == key){
                node->val = val;
                return;
            }
            node = node->next;
        }

        Node *nnode = new Node(key, val);
        nnode->next = table[idx];
        table[idx] = nnode;
        ++size;
    }

    bool find(const K& key, V& val){
        size_t idx = hash(key);
        Node *node = table[idx];

        while(node != nullptr){
            if(node->key == key){
                val = node->val;
                return true;
            }
            node = node->next;
        }
        return false;
    }

    bool erase(const K& key){
        size_t idx = hash(key);
        Node *node = table[idx];
        Node *prev = nullptr;
        
        while(node != nullptr){
            if(node->key == key){
                if(prev != nullptr){
                    prev->next = node->next;
                }else{
                    table[idx] = nullptr;
                }
                delete node;
                --size;
                return true;
            }
            prev = node;
            node = node->next;
        }
        return false;
    }

    ~HashMap(){
        for(auto elem : table){
            if(elem != nullptr){
                Node *p = elem;
                while(p != nullptr){
                    Node *next = p->next;
                    delete p;
                    p = next;
                }
            }
        }
    }

private:
    void resize(){
        capacity *= 2;
        vector<Node*> ntable(capacity, nullptr);

        for(auto elem : table){
            if(elem != nullptr){
                Node *p = elem;
                while(p != nullptr){
                    size_t idx = hash(p->key);
                    Node *next = p->next;
                    p->next = ntable[idx];
                    p = next;
                }
            }
        }

        table = move(ntable);
    }



private:

    struct Node{
        K key;
        V val;
        Node *next;
        
        Node(K k, V v): key(k), val(v), next(nullptr){}
    }

    size_t hash(const K& key) const{
        return hash<K>{}(key) % capacity;
    }

private:
    size_t size;
    size_t capacity;
    vector<Node*> table;

};