/*
https://leetcode.cn/problems/find-the-duplicate-number/description/?envType=problem-list-v2&envId=OUe7cMpw
287. 寻找重复数
给定一个包含 n + 1 个整数的数组 nums ，其数字都在 [1, n] 范围内（包括 1 和 n），可知至少存在一个重复的整数。

假设 nums 只有 一个重复的整数 ，返回 这个重复的数 。

你设计的解决方案必须 不修改 数组 nums 且只用常量级 O(1) 的额外空间 。

*/

//注意题目中数组范围和数字范围，表明一定存在环，做法同链表检测环起点

#include<vector>
using namespace std;

int findDuplicate(vector<int>& nums){
    int slow = 0, fast = 0;
    do{
        slow = nums[slow];
        fast = nums[nums[fast]];
    }while(slow != fast);

    fast = 0;
    while(slow != fast){
        slow = nums[slow];
        fast = nums[fast];
    }

    return fast;
}