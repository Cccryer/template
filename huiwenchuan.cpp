using namespace std;
#include<string>
#include<vector>

/*
    二维dp做回文串
    自己和自己对比，二维矩阵只看右上部分
    dp[i][j]表示s[i:j]是否是回文串，由于对称性，dp[i][j] = dp[i+1][j-1] && s[i] == s[j]
*/

void subhuiwen(string s){
    //回文子串 dp法
    int n = s.size();
    vector<vector<char>> dp(n, vector<char>(n, 1)); //注意这里初始化全为true
    //从右下角开始遍历
    for(int i = n-1; i >= 0; --i){
        for(int j = i + 1; j < n; ++j){ //j = i + 1非常巧妙，导致长度为1的不用判，长度为2的只对比s[i]==s[j]即可
            dp[i][j] = (s[i] == s[j]) && dp[i+1][j-1];
        }
    }
}

// 最长回文串
string longestPalindrome(string s) {
    int n = s.size();
    vector<vector<char>> dp(n, vector<char>(n, 1));
    int idx = 0, len = 1;

    for(int i = n-1; i >= 0; --i){
        for(int j = i + 1; j < n; ++j){
            dp[i][j] = (s[i] == s[j]) && dp[i+1][j-1];
            if(dp[i][j]){
                if(j - i + 1 > len){
                    idx = i;
                    len = j - i + 1;
                }
            }
        }
    }
    return s.substr(idx, len);
}

// 最长回文子序列
int longestPalindromeSubseq(string s) {
    int n = s.size();
    vector<vector<int>> dp(n, vector<int>(n));

    for(int i = n-1; i >= 0; --i){
        dp[i][i] = 1; //长度为1的回文子序列
        for(int j = i + 1; j < n; ++j){
            if(s[i] == s[j]){
                dp[i][j] = dp[i+1][j-1] + 2;
            }else{
                dp[i][j] = max(dp[i+1][j], dp[i][j-1]);
            }
        }
    }
    return dp[0][n-1];
}