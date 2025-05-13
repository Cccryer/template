#include <vector>
#include <algorithm>
#include <limits.h>
using namespace std;
/*
买卖股票的最佳时机专题
*/


/*
最多买卖2次
给定一个数组，它的第 i 个元素是一支给定的股票在第 i 天的价格。

设计一个算法来计算你所能获取的最大利润。你最多可以完成 两笔 交易。

注意：你不能同时参与多笔交易（你必须在再次购买前出售掉之前的股票）。
*/

int maxProfit(vector<int>& prices) {
    int n = prices.size();

    vector<vector<int>> dp(n, vector<int>(4));

    dp[0][0] = -prices[0];
    dp[0][1] = 0;
    dp[0][2] = -prices[0];
    dp[0][3] = 0;

    for(int i = 1; i < n; ++i){
        dp[i][0] = max(dp[i-1][0], 0 - prices[i]); //这里不能是dp[i-1][1] - prives[i] 因为有第一支股票状态不能再没有第一支股票状态后
        dp[i][1] = max(dp[i-1][1], dp[i-1][0] + prices[i]);
        dp[i][2] = max(dp[i-1][2], dp[i-1][1] - prices[i]);
        dp[i][3] = max(dp[i-1][3], dp[i-1][2] + prices[i]);
    }

    return dp[n-1][3];

}
/*
最多卖卖k次
*/
int maxProfit2(vector<int>& prices, int k) {
    int n = prices.size();
    vector<vector<int>> dp(n, vector<int>(k*2));

    for(int j = 0; j < n; ++j){
        if(j % 2 == 0){
            dp[0][j] = -prices[0]; 
        }
    }
    
    for(int i = 1; i < n; ++i){
        for(int j = 0; j < k*2; ++j){
            if(j % 2 == 0){
                dp[i][j] = max(dp[i-1][j], dp[i-1][j-1] - prices[i]);
            }else{
                dp[i][j] = max(dp[i-1][j], dp[i-1][j-1] + prices[i]);
            }
        }
    }

    return dp[n-1][k*2-1];
}

/*
任意次买卖
*/
int maxProfit(vector<int>& prices){
    int n = prices.size();
    vector<vector<int>> dp(n, vector<int>(2));

    dp[0][0] = -prices[0];
    dp[0][1] = 0;

    for(int i = 1; i < n; ++i){
        dp[i][0] = max(dp[i-1][0], dp[i-1][1] - prices[i]); //由于不限制买卖次数，所以这里可以直接用前一天的卖出状态来买入
        dp[i][1] = max(dp[i-1][1], dp[i-1][0] + prices[i]);
    }

    return dp[n-1][1];
}

/*
只能买一次
*/

int maxProfit(vector<int>& prices) {
    
    int n = prices.size();
    int buy = INT_MIN, sell = INT_MIN;
    for(int i = 0; i < n; ++i){
        buy = max(buy, -prices[i]); // 这里不能是dp[i-1][1] - prices[i] 因为只能买卖一次
        sell = max(sell, buy + prices[i]);
    }

    return sell;
}