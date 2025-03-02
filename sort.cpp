using namespace std;
#include<vector>
#include<iostream>
void quick_sort(vector<int> &nums, int l, int r){
    if(l>=r)
        return;
    int i=l-1,j=r+1;
    int k = nums[(l+r)/2];
    while(i<j)
    {
        while(nums[++i] < k);
        while(nums[--j] > k);
        if(i<j)
            swap(nums[i], nums[j]);
    }
    quick_sort(nums,l,j);
    quick_sort(nums,j+1,r);
}
int quick_select(vector<int> &nums, int l, int r, int k){
    if(l == r)
        return nums[l];
    int i = l - 1, j = r + 1;
    int mid = nums[(l+r)/2];
    while(i < j){
        while(nums[++i] < mid);
        while(nums[--j] > mid);
        if(i< j) 
            swap(nums[i], nums[j]);
    }
    if(k <= j)
        return quick_select(nums, l, j, k);
    else   
        return quick_select(nums, j+1, r, k);
}
int main(){
    vector<int> nums = {1, 8, 6, 4, 0, 4, 8, 10, 3, 1, 4, 8, 9};
    // quick_sort(nums, 0, nums.size()-1);
    // for(int i = 0; i < nums.size(); i++){
    //     cout<<nums[i]<<' ';
    // }
    cout<<quick_select(nums, 0, nums.size()-1, nums.size()-3);
    return 0;
}