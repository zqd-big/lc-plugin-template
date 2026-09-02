/*
 * @lc app=leetcode.cn id=1 lang=c
 *
 * [1] 两数之和
 */

#include "../common/lc_common.h"

// @lc code=start
typedef struct NumberIndex {
    int key;
    int index;
    UT_hash_handle hh;
} NumberIndex;

int *twoSum(int *nums, int numsSize, int target, int *returnSize)
{
    NumberIndex *table = NULL;
    NumberIndex *entry = NULL;
    NumberIndex *temporary = NULL;
    int *answer = NULL;
    int index;

    *returnSize = 0;
    for (index = 0; index < numsSize; ++index) {
        int complement = target - nums[index];
        HASH_FIND_INT(table, &complement, entry);
        if (entry != NULL) {
            answer = (int *)malloc(2u * sizeof(*answer));
            if (answer != NULL) {
                answer[0] = entry->index;
                answer[1] = index;
                *returnSize = 2;
            }
            break;
        }

        HASH_FIND_INT(table, &nums[index], entry);
        if (entry == NULL) {
            entry = (NumberIndex *)malloc(sizeof(*entry));
            if (entry == NULL) {
                break;
            }
            entry->key = nums[index];
            entry->index = index;
            HASH_ADD_INT(table, key, entry);
        }
    }

    HASH_ITER(hh, table, entry, temporary) {
        HASH_DEL(table, entry);
        free(entry);
    }
    return answer;
}
// @lc code=end

int main(void)
{
    int nums[] = { 2, 7, 11, 15 };
    int returnSize = 0;
    int *answer = twoSum(nums, (int)LC_ARRAY_SIZE(nums), 9, &returnSize);

    assert(answer != NULL);
    assert(returnSize == 2);
    assert(answer[0] == 0 && answer[1] == 1);
    printf("[%d, %d]\n", answer[0], answer[1]);
    free(answer);
    return 0;
}
