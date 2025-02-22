#include<functional>
//大顶堆用less，小顶堆用greater
//compare(parent，child)，注意比较参数顺序
template<typename T, typename Cmp = std::less<T>>
    class Heap{
    public:
        Heap() = default;
        explicit Heap(Cmp _compare): compare(_compare){}

        void push(const T& val){
            heap.push_back(val);
            up(heap.size()-1);
        }

        void pop(){
            if(!heap.empty()){
                swap(heap.front(), heap.back());
                heap.pop_back();
                if(!heap.empty()){
                    down(0);
                }
            }
        }

        const T& top(){
            return heap.front();
        }

        size_t size(){
            return heap.size();
        }

        bool empty(){
            return heap.empty();
        }

    private:
        Cmp compare;
        vector<T> heap;

    private:
        void up(size_t pos){
            while(pos > 0){
                size_t parent = (pos-1)/2;
                if(compare(heap[parent], heap[pos])){
                    swap(heap[parent], heap[pos]);
                    pos = parent;
                }else{
                    break;
                }
            }
        }

        void down(size_t pos){
            int sz = heap.size();
            while(pos < sz){
                size_t left = pos*2 + 1;
                size_t right = pos*2 + 2;
                size_t rel = pos;
                if(left < sz && compare(heap[rel], heap[left])){
                    rel = left;
                }
                if(right < sz && compare(heap[rel], heap[right])){
                    rel = right;
                }
                if(rel != pos){
                    swap(heap[rel], heap[pos]);
                    pos = rel;
                }else{
                    break;
                }
            }
        }
    };