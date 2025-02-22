#include<vector>
#include<iostream>
using namespace std;
class DisjionSets{
    private:
        int count;//节点数
        vector<int> size;//联通分量大小
        vector<int> parent;//
    public:
        DisjionSets(int count){
            this->count = count;
            parent.resize(count);
            size.resize(count);
            for(int i = 0; i < count; i++){
                parent[i] = i;
                size[i] = 1;
            }
        }
        ~DisjionSets(){}

        int find(int x);
        void uunion(int x, int y);
        bool connected(int x, int y);
        int getCount();
        int maxConnectSize();
};
int DisjionSets::find(int x){
    if(this->parent[x] != x){
        this->parent[x] = find(this->parent[x]);
    }
    return this->parent[x];
}
void DisjionSets::uunion(int x, int y){
    if(find(x) == find(y))
        return;
    int rootx = find(x);
    int rooty = find(y);
    this->parent[rootx] = rooty;
    this->size[rooty] += this->size[rootx];
    this->count--;
}
bool DisjionSets::connected(int x, int y){
    return find(x) == find(y);
}
int DisjionSets::getCount(){
    return this->count;
}
int DisjionSets::maxConnectSize(){
    int maxsize = 0;
    for(int i = 0; i < this->parent.size(); i++){
        if(i == parent[i])
            maxsize = max(maxsize, size[i]);
    }
    return maxsize;
}
int main(){
    DisjionSets djs(10);
    return 0;
}