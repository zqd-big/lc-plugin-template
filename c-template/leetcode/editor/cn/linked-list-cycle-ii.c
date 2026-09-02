/*
 * @lc app=leetcode.cn id=142 lang=c
 * @lcpr version=30404
 *
 * [142] 环形链表 II
 */

#include "../common/lc_common.h"

// @lc code=start
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode *detectCycle(struct ListNode *head) {
    
}
// @lc code=end

int main(void) {
    /* 在这里写本地测试代码；main 不会被提交到力扣。 */
    VosVector *vec = VOS_VectorCreate(sizeof(int));
    int a = 1;
    VOS_VectorPushBack(vec, &a);
    printf("hello world\n");
    return 0;
}



/*
// @lcpr case=start
// [3,2,0,-4]\n1\n
// @lcpr case=end

// @lcpr case=start
// [1,2]\n0\n
// @lcpr case=end

// @lcpr case=start
// [1]\n-1\n
// @lcpr case=end

 */

