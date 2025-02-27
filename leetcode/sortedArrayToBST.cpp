/*
https://leetcode.cn/problems/convert-sorted-array-to-binary-search-tree/?envType=problem-list-v2&envId=OUe7cMpw
给你一个整数数组 nums ，其中元素已经按 升序 排列，请你将其转换为一棵 平衡 二叉搜索树。
*/
#include<vector>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};


TreeNode* sortedArrayToBST(vector<int>& nums){
    return traverse(nums, 0, nums.size());
}

TreeNode* traverse(vector<int>& nums, int l, int r){
    if(l >= r)
        return nullptr;

    int mid = l + r >> 1;
    TreeNode *root = new TreeNode(nums[mid]);

    root->left = traverse(nums, l, mid);
    root->right = traverse(nums, mid + 1, r);

    return root;
}
