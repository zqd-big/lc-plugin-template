#include "lc_common.h"

#include <assert.h>
#include <stdio.h>

static int gFreedStrings = 0;
static int gFreedQueueValues = 0;

static int32_t CompareIntData(const void *left, const void *right)
{
    int leftValue = *(const int *)left;
    int rightValue = *(const int *)right;
    return (leftValue > rightValue) - (leftValue < rightValue);
}

static int32_t CompareMinInt(uintptr_t left, uintptr_t right)
{
    return VOS_IntCmpFunc(right, left);
}

static int32_t CompareIntPointer(uintptr_t left, uintptr_t right)
{
    int leftValue = *(const int *)left;
    int rightValue = *(const int *)right;
    return (leftValue > rightValue) - (leftValue < rightValue);
}

static char *CopyString(const char *source)
{
    size_t length = strlen(source) + 1u;
    char *copy = (char *)malloc(length);
    if (copy != NULL) {
        memcpy(copy, source, length);
    }
    return copy;
}

static void FreeStringSlot(void *slot)
{
    char **text = (char **)slot;
    free(*text);
    *text = NULL;
    gFreedStrings += 1;
}

static void *DuplicateInt(void *source)
{
    int *copy = (int *)malloc(sizeof(*copy));
    if (copy != NULL) {
        *copy = *(const int *)source;
    }
    return copy;
}

static void FreeQueueInt(void *value)
{
    free(value);
    gFreedQueueValues += 1;
}

static void TestVector(void)
{
    const int input[] = { 5, 1, 4, 2, 3 };
    const int expected[] = { 1, 2, 3, 4, 5 };
    VOS_VECTOR *vector = VOS_VectorCreate(sizeof(int));
    size_t index;
    int needle = 4;

    assert(vector != NULL);
    for (index = 0; index < LC_ARRAY_SIZE(input); ++index) {
        assert(VOS_VectorPushBack(vector, &input[index]) == VOS_OK);
    }
    VOS_VectorSort(vector, CompareIntData);
    for (index = 0; index < LC_ARRAY_SIZE(expected); ++index) {
        assert(*(int *)VOS_VectorAt(vector, index) == expected[index]);
    }
    assert(*(int *)VOS_VectorSearch(vector, &needle, CompareIntData) == 4);
    assert(VOS_VectorErase(vector, 2u) == VOS_OK);
    assert(*(int *)VOS_VectorAt(vector, 2u) == 4);
    assert(VOS_VectorAt(vector, 99u) == NULL);

    VOS_VectorClear(vector, NULL);
    assert(VOS_VectorSize(vector) == 0u);
    assert(VOS_VectorPushBack(vector, &needle) == VOS_OK);
    assert(*(int *)VOS_VectorAt(vector, 0u) == needle);
    VOS_VectorDestroy(vector, NULL);

    vector = VOS_VectorRawCreate(sizeof(int), 1u, 1u);
    assert(vector != NULL);
    assert(VOS_VectorPushBack(vector, &input[0]) == VOS_OK);
    /* The source lives inside a full vector and must survive realloc. */
    assert(VOS_VectorPushBack(vector, VOS_VectorAt(vector, 0u)) == VOS_OK);
    assert(VOS_VectorSize(vector) == 2u);
    assert(*(int *)VOS_VectorAt(vector, 1u) == input[0]);
    VOS_VectorClear(vector, NULL);
    for (index = 0; index < LC_ARRAY_SIZE(input); ++index) {
        assert(VOS_VectorPushBack(vector, &input[index]) == VOS_OK);
    }
    VOS_VectorDestroy(vector, NULL);

    vector = VOS_VectorCreate(sizeof(char *));
    assert(vector != NULL);
    {
        char *first = CopyString("alpha");
        char *second = CopyString("beta");
        assert(first != NULL && second != NULL);
        assert(VOS_VectorPushBack(vector, &first) == VOS_OK);
        assert(VOS_VectorPushBack(vector, &second) == VOS_OK);
    }
    VOS_VectorDestroy(vector, FreeStringSlot);
    assert(gFreedStrings == 2);
}

static void TestPriorityQueue(void)
{
    const uintptr_t values[] = { 7u, 3u, 9u, 9u, 1u };
    VOS_PROPRIQUEUE *maxQueue = VOS_PriQueCreate(VOS_IntCmpFunc, NULL);
    VosPriQue *minQueue = VOS_PriQueCreate(CompareMinInt, NULL);
    size_t index;

    assert(maxQueue != NULL && minQueue != NULL);
    assert(VOS_IntCmpFunc(9u, 3u) > 0);
    assert(VOS_StrCmpFunc((uintptr_t)"beta", (uintptr_t)"alpha") > 0);
    for (index = 0; index < LC_ARRAY_SIZE(values); ++index) {
        assert(VOS_PriQuePush(maxQueue, values[index]) == VOS_OK);
        assert(VOS_PriQuePush(minQueue, values[index]) == VOS_OK);
    }
    assert(VOS_PriQueSize(maxQueue) == LC_ARRAY_SIZE(values));
    assert(VOS_PriQueTop(maxQueue) == 9u);
    VOS_PriQuePop(maxQueue);
    assert(VOS_PriQueTop(maxQueue) == 9u);
    assert(VOS_PriQueTop(minQueue) == 1u);
    VOS_PriQueClear(minQueue);
    assert(VOS_PriQueEmpty(minQueue));
    assert(VOS_PriQuePush(minQueue, 6u) == VOS_OK);
    assert(VOS_PriQueTop(minQueue) == 6u);
    VOS_PriQueDestroy(minQueue);
    VOS_PriQueDestroy(maxQueue);

    maxQueue = VOS_PriQueCreate(VOS_IntCmpFunc, NULL);
    assert(maxQueue != NULL);
    {
        int noCopyBatch[] = { 1, 2 };
        assert(VOS_PriQuePushBatch(
            maxQueue,
            noCopyBatch,
            LC_ARRAY_SIZE(noCopyBatch),
            sizeof(noCopyBatch[0])
        ) == VOS_ERROR);
        assert(VOS_PriQueEmpty(maxQueue));
    }
    VOS_PriQueDestroy(maxQueue);

    {
        int batch[] = { 4, 10, 2 };
        int extra = 8;
        VosDupFreeFuncPair functions = { DuplicateInt, FreeQueueInt };
        VosPriQue *ownedQueue = VOS_PriQueCreate(CompareIntPointer, &functions);

        assert(ownedQueue != NULL);
        assert(VOS_PriQuePushBatch(
            ownedQueue,
            batch,
            LC_ARRAY_SIZE(batch),
            sizeof(batch[0])
        ) == VOS_OK);
        assert(*(int *)VOS_PriQueTop(ownedQueue) == 10);
        VOS_PriQuePop(ownedQueue);
        assert(gFreedQueueValues == 1);
        assert(VOS_PriQuePush(ownedQueue, (uintptr_t)&extra) == VOS_OK);
        assert(*(int *)VOS_PriQueTop(ownedQueue) == 8);
        VOS_PriQueDestroy(ownedQueue);
        assert(gFreedQueueValues == 4);
    }
}

typedef struct HashEntry {
    int key;
    int count;
    UT_hash_handle hh;
} HashEntry;

static void TestUthash(void)
{
    const int values[] = { 2, 1, 2, 3, 2, 1 };
    HashEntry *table = NULL;
    HashEntry *entry;
    HashEntry *temporary;
    size_t index;
    int key = 2;

    for (index = 0; index < LC_ARRAY_SIZE(values); ++index) {
        HASH_FIND_INT(table, &values[index], entry);
        if (entry == NULL) {
            entry = (HashEntry *)calloc(1, sizeof(*entry));
            assert(entry != NULL);
            entry->key = values[index];
            HASH_ADD_INT(table, key, entry);
        }
        entry->count += 1;
    }
    assert(HASH_COUNT(table) == 3u);
    HASH_FIND_INT(table, &key, entry);
    assert(entry != NULL && entry->count == 3);

    HASH_ITER(hh, table, entry, temporary) {
        HASH_DEL(table, entry);
        free(entry);
    }
    assert(table == NULL);
}

static void TestNodes(void)
{
    const int values[] = { 1, 2, 3 };
    LcTreeItem treeItems[] = {
        LC_TREE_VALUE(1),
        LC_TREE_NULL,
        LC_TREE_VALUE(2),
        LC_TREE_VALUE(3)
    };
    struct ListNode *list = LC_ListFromArray(values, LC_ARRAY_SIZE(values));
    struct TreeNode *tree = LC_TreeFromLevelOrder(treeItems, LC_ARRAY_SIZE(treeItems));

    assert(LC_ListEqualsArray(list, values, LC_ARRAY_SIZE(values)));
    LC_ListDestroy(list);
    assert(tree != NULL && tree->val == 1);
    assert(tree->left == NULL);
    assert(tree->right != NULL && tree->right->val == 2);
    assert(tree->right->left != NULL && tree->right->left->val == 3);
    LC_TreeDestroy(tree);
}

int main(void)
{
    TestVector();
    TestPriorityQueue();
    TestUthash();
    TestNodes();
    puts("All C template tests passed.");
    return 0;
}
