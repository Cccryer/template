//下一个排列
//https://leetcode.cn/problems/next-permutation/?envType=problem-list-v2&envId=OUe7cMpw

/*
从右向左找到第一个降序的位置：也就是找到第一个索引i，使得nums[i] < nums[i+1]。
此时，i右边的部分是降序排列的，已经是最大的可能排列，无法通过交换右边的元素得到更大的排列，所以需要调整i位置的元素。
在右边找到比nums[i]大的最小元素：找到右边第一个比nums[i]大的元素nums[j]，交换这两个元素。这样能确保新的排列比原来大，但尽可能小。
*/
#include<vector>
#include<algorithm>
using namespace std;
void nextPermutation(vector<int>& nums){
    //升序最前
    //降序最后
    //从后往前找第一个不降序的，后面就是降序也就是最后了
    //从中找到比前一个大一点的最小

    int n = nums.size();
    for(int i = n -1; i > 0; --i){
        if(nums[i-1] < nums[i]){
            for(int j = n-1; j >= i; --j){
                if(nums[j] > nums[i-1]){
                    swap(nums[j], nums[i-1]);
                    sort(nums.begin()+i, nums.end());
                    return;
                }
            }
        }
    }
    sort(nums.begin(), nums.end());
}

