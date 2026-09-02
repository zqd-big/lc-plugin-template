#include "vos_common.h"

#include <string.h>

int32_t VOS_IntCmpFunc(uintptr_t data1, uintptr_t data2)
{
    if (data1 > data2) {
        return 1;
    }
    if (data1 < data2) {
        return -1;
    }
    return 0;
}

int32_t VOS_StrCmpFunc(uintptr_t addr1, uintptr_t addr2)
{
    const char *left = (const char *)addr1;
    const char *right = (const char *)addr2;

    if (left == right) {
        return 0;
    }
    if (left == NULL) {
        return -1;
    }
    if (right == NULL) {
        return 1;
    }
    return (int32_t)strcmp(left, right);
}
