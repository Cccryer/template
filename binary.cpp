using namespace std;
#include<vector>
int binary(vector<int> nums, int target){
    int l = 0, r = nums.size();
    while(l < r){
        int mid = l + r>>1;
        if(nums[mid] == target)
            return mid;
        if(nums[mid] > target)
            r = mid;
        else{
            l = mid+1;
        }
    }
    return -1;
}


int binary1(vector<int> nums, int target){
    int l = 0, r = nums.size()-1;
    while(l < r){
        int mid = l + r >> 1;
        if(nums[mid] >= target)
            r = mid;
        else
            l = mid + 1;
    }
    return nums[l]==target?l:-1;
}