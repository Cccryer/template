#include<vector>

using namespace std;

#define INT_MAX 2147483647
#define INT_MIN -2147483648

//各种topk算法

//找无序数组的topK
//用快速选择
int findTopK(vector<int>& nums, int l, int r, int k){
    if(l >= r){
        return nums[l];
    }

    int i = l-1, j = r + 1;
    int mid = nums[(l + r)/ 2];
    while(i < j){
        while(nums[++i] < mid);
        while(nums[--j] > mid);
        if(i < j)
            swap(nums[i], nums[j]);
    }

    if(k <= j)
        return findTopK(nums, l , j, k);
    return findTopK(nums, j+1, r, k);
}


int count(vector<int> nums1, vector<int> nums2, int x);
int binary(vector<int>& nums, int x);

//两个有序数组找topK
int findTopK(vector<int> nums1, vector<int> nums2, int k){
    
    //枚举答案，从最小到最大中
    /*
        不过，读到这里，有的读者可能会有疑问：如何确定 topk 二分的答案一定在两个数组中存在呢？ 
        其实从 count(x) 的函数图像可以知道，所有的拐点都是数组中的元素，而满足二分条件的左界一定在拐点上，
        所以二分答案一定在两个数组中存在。 子函数 count(x) 本身的设计，就保证了拐点一定是数组中的元素

        转载请注明原文链接: https://writings.sh/post/binary-search
    */

    int l = min(nums1.empty() ? INT_MAX : nums1[0],
                nums2.empty() ? INT_MAX : nums2[0]);
    
    int r = max(nums1.empty() ? INT_MIN : nums1[nums1.size()-1],
                nums2.empty() ? INT_MIN : nums2[nums2.size()-1]);
    
    //count(x)为非递减函数
    //找count(x) >= k性质的左边界
    while(l < r){
        int mid = (l + r) >> 1;
        if(count(nums1, nums2, mid) >= k){ 
            r = mid;
        }else{
            l = mid + 1;
        }
    }
    
    return l;
}

//计算非递减序列nums1和nums2中小于等于x的元素数量
int count(vector<int> nums1, vector<int> nums2, int x){
    int res = 0;
    if(nums1.size() > 0 && nums1[0] <= x){
        res += binary(nums1, x);
    }

    if(nums2.size() > 0 && nums2[0] <= x){
        res += binary(nums2, x);
    }
    return res;
}

//计算nums中小于等于x的元素数量， 必须保证nums中存在小于等于x的元素
int binary(vector<int>& nums, int x){
    int l = 0, r = nums.size()-1;

    while(l < r){
        int mid = (l + r + 1) >> 1;
        if(nums[mid] <= x){
            l = mid;
        }else{
            r = mid - 1;
        }
    }
    //数量是下标+1
    return l+1;
}

//两个有序数组找中位数
//复用topK
double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
    int m = nums1.size(), n = nums2.size();
    if(m == 0 && n == 0)
        return 0;
    int q = m + n;

    if(q & 1){
        return findTopK(nums1, nums2, (q + 1)/2);
    }else{
        int a = findTopK(nums1, nums2, q / 2);
        int b = findTopK(nums1, nums2, q / 2 + 1);
        return (a + b) / 2.0;
    }
}

//扩展到n个有序数组topK也是简单的，只需修改count和findTopK的查找数