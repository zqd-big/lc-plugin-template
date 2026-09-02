#include "lc_nodes.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct ListNode *LC_ListFromArray(const int *values, size_t count)
{
    struct ListNode dummy = { 0, NULL };
    struct ListNode *tail = &dummy;
    size_t index;

    if (values == NULL && count != 0u) {
        return NULL;
    }
    for (index = 0; index < count; ++index) {
        struct ListNode *node = (struct ListNode *)malloc(sizeof(*node));
        if (node == NULL) {
            LC_ListDestroy(dummy.next);
            return NULL;
        }
        node->val = values[index];
        node->next = NULL;
        tail->next = node;
        tail = node;
    }
    return dummy.next;
}

bool LC_ListEqualsArray(const struct ListNode *head, const int *values, size_t count)
{
    size_t index = 0u;

    if (values == NULL && count != 0u) {
        return false;
    }
    while (head != NULL && index < count) {
        if (head->val != values[index]) {
            return false;
        }
        head = head->next;
        index += 1u;
    }
    return head == NULL && index == count;
}

void LC_ListPrint(const struct ListNode *head)
{
    putchar('[');
    while (head != NULL) {
        printf("%d", head->val);
        if (head->next != NULL) {
            printf(", ");
        }
        head = head->next;
    }
    puts("]");
}

void LC_ListDestroy(struct ListNode *head)
{
    while (head != NULL) {
        struct ListNode *next = head->next;
        free(head);
        head = next;
    }
}

struct TreeNode *LC_TreeFromLevelOrder(const LcTreeItem *items, size_t count)
{
    struct TreeNode *root;
    struct TreeNode **queue;
    size_t head = 0u;
    size_t tail = 0u;
    size_t index = 1u;

    if (items == NULL || count == 0u || items[0].isNull) {
        return NULL;
    }
    if (count > SIZE_MAX / sizeof(*queue)) {
        return NULL;
    }
    root = (struct TreeNode *)calloc(1, sizeof(*root));
    queue = (struct TreeNode **)malloc(count * sizeof(*queue));
    if (root == NULL || queue == NULL) {
        free(root);
        free(queue);
        return NULL;
    }
    root->val = items[0].value;
    queue[tail++] = root;

    while (head < tail && index < count) {
        struct TreeNode *parent = queue[head++];

        if (!items[index].isNull) {
            parent->left = (struct TreeNode *)calloc(1, sizeof(*parent->left));
            if (parent->left == NULL) {
                LC_TreeDestroy(root);
                free(queue);
                return NULL;
            }
            parent->left->val = items[index].value;
            queue[tail++] = parent->left;
        }
        index += 1u;
        if (index < count && !items[index].isNull) {
            parent->right = (struct TreeNode *)calloc(1, sizeof(*parent->right));
            if (parent->right == NULL) {
                LC_TreeDestroy(root);
                free(queue);
                return NULL;
            }
            parent->right->val = items[index].value;
            queue[tail++] = parent->right;
        }
        index += 1u;
    }
    free(queue);
    return root;
}

static void LC_TreePrintIndented(const struct TreeNode *node, size_t depth, const char *edge)
{
    size_t index;

    if (node == NULL) {
        return;
    }
    LC_TreePrintIndented(node->right, depth + 1u, "/");
    for (index = 0; index < depth; ++index) {
        printf("    ");
    }
    printf("%s%d\n", edge, node->val);
    LC_TreePrintIndented(node->left, depth + 1u, "\\");
}

void LC_TreePrint(const struct TreeNode *root)
{
    if (root == NULL) {
        puts("(empty tree)");
        return;
    }
    LC_TreePrintIndented(root, 0u, "");
}

void LC_TreeDestroy(struct TreeNode *root)
{
    if (root == NULL) {
        return;
    }
    LC_TreeDestroy(root->left);
    LC_TreeDestroy(root->right);
    free(root);
}
