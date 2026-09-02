#ifndef LABULADONG_LC_NODES_H
#define LABULADONG_LC_NODES_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ListNode {
    int val;
    struct ListNode *next;
};

struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

typedef struct {
    bool isNull;
    int value;
} LcTreeItem;

#define LC_TREE_VALUE(number) ((LcTreeItem){ false, (number) })
#define LC_TREE_NULL ((LcTreeItem){ true, 0 })
#define LC_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

struct ListNode *LC_ListFromArray(const int *values, size_t count);
bool LC_ListEqualsArray(const struct ListNode *head, const int *values, size_t count);
void LC_ListPrint(const struct ListNode *head);
void LC_ListDestroy(struct ListNode *head);

struct TreeNode *LC_TreeFromLevelOrder(const LcTreeItem *items, size_t count);
void LC_TreePrint(const struct TreeNode *root);
void LC_TreeDestroy(struct TreeNode *root);

#ifdef __cplusplus
}
#endif

#endif /* LABULADONG_LC_NODES_H */
