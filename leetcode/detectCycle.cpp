//环形链表找起点
//https://leetcode.cn/problems/linked-list-cycle-ii/description/

        //当s和f相遇时：
        //1. fast走的一定是slow的两倍  f = 2s
        //2. fast比slow多走整数圈     f = s + nc  -> s = nc
        //3. 推出s一定是环的整数倍
        //4. 又：如果走到环起点，走的距离一定是a + kc
        //5. 所以s再走a（所求位置到起点距离）一定到所求位置
        //6. s再走a一定到环起点<->head走a也到环起点 ->计数环起点距离
        /*

        关 建 在 于 ：慢 指 针 所 走 距 离 一 定 是 环 的 整 数 倍*********
                     走 到 起 点 的 距 离 一 定 是 所 求 加 环 的 整 数 倍
        */


struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode *detectCycle(ListNode *head){

    ListNode *slow = head;
    ListNode *fast = head;
    
    do{
        if(fast == nullptr || fast->next == nullptr)
                return nullptr;
        slow = slow->next;
        fast = fast->next->next;
    }while(slow != fast);

    fast = head;

    while(fast != slow){
        fast = fast->next;
        slow = slow->next;
    }

    return fast;
}