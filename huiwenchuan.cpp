using namespace std;
#include<string>
#include<vector>
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