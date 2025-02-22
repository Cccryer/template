//字符串匹配 1. kmp 2. 字符串哈希
#include<string>
#include<vector>
using std::string;
using std::vector;
int kmp(string haystack, string needle)
{
    if(needle.size() > haystack.size())
        return -1;
    vector<int> next(needle.size(), 0);
    int i = 0, j = 1;
    while(j < needle.size())
    {
        while(i > 0 && needle[i] != needle[j])
            i = next[i-1];
        if(needle[i] == needle[j])
            ++i;
        next[j] = i;
        ++j;
    }
    i = 0, j = 0;
    while(i < haystack.size())
    {
        while(i < haystack.size() && haystack[i] == needle[j])
        {
            ++i;
            ++j;
            if(j == needle.size())
                return i - j;
        }
        if(j == 0)
        {
            ++i;
            break;
        }
        j = next[j-1];
    }
    return -1;
}

int strhash(string haystack, string needle)
{
    //字符串哈希做法
    if(needle.size() > haystack.size())
        return -1;
    typedef unsigned long long ULL;
    const int P = 131,  m = haystack.size(), n = needle.size();
    vector<ULL> p(m+1), h(m+1);
    p[0] = 1, h[0] = 0;

    for(int i = 1; i <= m; ++i)
    {
        p[i] = p[i-1]*P;
        h[i] = h[i-1]*P + haystack[i-1];
    }

    ULL target = 0;
    for(int i = 1;i <= n; ++i)
        target = target*P + needle[i-1];

    for(int i = 0; i <= m-n; ++i)
    {
        if(h[i+n] - h[i]*p[n] == target)
            return i;
    }
    return -1;
}