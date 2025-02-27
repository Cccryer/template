/*
和为 K 的子数组
https://leetcode.cn/problems/subarray-sum-equals-k/description/
给你一个整数数组 nums 和一个整数 k ，请你统计并返回 该数组中和为 k 的子数组的个数 。

子数组是数组中元素的连续非空序列。

*/

#include<vector>
#include<unordered_map>
using namespace std;
int subarraySum(vector<int>& nums, int k) {

    unordered_map<int, int> mp;
    int sum = 0;
    int res = 0;
    mp[0] = 1; //cover sum 直接等于k的情况
    for(int i = 0; i < nums.size(); ++i){
        sum += nums[i];

        if(mp.find(sum - k) != mp.end()){
            res += mp[sum-k];
        }

        ++mp[sum];
    }
    return res;

}

/*
路径总和 III
给定一个二叉树的根节点 root ，和一个整数 targetSum ，求该二叉树里节点值之和等于 targetSum 的 路径 的数目。
路径 不需要从根节点开始，也不需要在叶子节点结束，但是路径方向必须是向下的（只能从父节点到子节点）。
https://leetcode.cn/problems/path-sum-iii/description/?envType=problem-list-v2&envId=OUe7cMpw
*/

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

int pathSum(TreeNode* root, int targetSum) {
    
    unordered_map<long long, int> mp;
    mp[0] = 1;
    int res = 0;
    traverse(root, targetSum, 0, mp, res);
    return res;

}

void traverse(TreeNode *root, int targetSum, long long sum, unordered_map<long long, int>& mp, int& res){
    if(root == nullptr)
        return;
    
    sum += root->val;
    if(mp.find(sum - targetSum) != mp.end()){
        res += mp[sum - targetSum];
    }

    ++mp[sum];
    traverse(root->left, targetSum, sum, mp, res);
    traverse(root->right, targetSum, sum, mp, res);
    --mp[sum];
}