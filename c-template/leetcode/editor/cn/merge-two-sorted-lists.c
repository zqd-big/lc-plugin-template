/*
 * @lc app=leetcode.cn id=21 lang=c
 *
 * [21] 合并两个有序链表
 */

#include "../common/lc_common.h"

// @lc code=start
struct ListNode *mergeTwoLists(struct ListNode *list1, struct ListNode *list2)
{
    struct ListNode dummy = { 0, NULL };
    struct ListNode *tail = &dummy;

    while (list1 != NULL && list2 != NULL) {
        if (list1->val <= list2->val) {
            tail->next = list1;
            list1 = list1->next;
        } else {
            tail->next = list2;
            list2 = list2->next;
        }
        tail = tail->next;
    }
    tail->next = list1 != NULL ? list1 : list2;
    return dummy.next;
}
// @lc code=end

int main(void)
{
    const int leftValues[] = { 1, 2, 4 };
    const int rightValues[] = { 1, 3, 4 };
    const int expected[] = { 1, 1, 2, 3, 4, 4 };
    struct ListNode *left = LC_ListFromArray(leftValues, LC_ARRAY_SIZE(leftValues));
    struct ListNode *right = LC_ListFromArray(rightValues, LC_ARRAY_SIZE(rightValues));
    struct ListNode *merged = mergeTwoLists(left, right);

    assert(LC_ListEqualsArray(merged, expected, LC_ARRAY_SIZE(expected)));
    LC_ListPrint(merged);
    /* The merged list owns all nodes; destroy it exactly once. */
    LC_ListDestroy(merged);
    return 0;
}
