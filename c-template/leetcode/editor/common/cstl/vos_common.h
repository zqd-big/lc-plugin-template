#ifndef VOS_COMMON_H
#define VOS_COMMON_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VOS_OK 0u
#define VOS_ERROR 1u

typedef int32_t (*VosDataCmpFunc)(const void *data1, const void *data2);
typedef int32_t (*VosKeyCmpFunc)(uintptr_t key1, uintptr_t key2);
typedef void *(*VosDupFunc)(void *ptr);
typedef void (*VosFreeFunc)(void *ptr);

typedef struct {
    VosDupFunc dupFunc;
    VosFreeFunc freeFunc;
} VosDupFreeFuncPair;

int32_t VOS_IntCmpFunc(uintptr_t data1, uintptr_t data2);
int32_t VOS_StrCmpFunc(uintptr_t addr1, uintptr_t addr2);

#define VOS_CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

static inline const char *VOS_CstlVersion(void)
{
    return "2.3-compatible (labuladong C template)";
}

#ifdef __cplusplus
}
#endif

#endif /* VOS_COMMON_H */
