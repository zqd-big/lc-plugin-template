#ifndef VOS_PRIORITYQUEUE_H
#define VOS_PRIORITYQUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "vos_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* cmp(data1, data2) > 0 means data1 has higher priority than data2. */
typedef int32_t (*VosPriQueCmpFunc)(uintptr_t data1, uintptr_t data2);

typedef struct tagVosPriQue VosPriQue;

/* Canonical and legacy compatibility names. */
typedef VosPriQue VOS_PRIORITY_QUEUE;
typedef VosPriQue VOS_PRIORITYQUEUE;
typedef VosPriQue VOS_PRIQUEUE;
typedef VosPriQue VOS_PROPRIQUEUE;

VosPriQue *VOS_PriQueCreate(VosPriQueCmpFunc cmpFunc, VosDupFreeFuncPair *dataFunc);
uint32_t VOS_PriQuePush(VosPriQue *priQueue, uintptr_t value);
uint32_t VOS_PriQuePushBatch(
    VosPriQue *priQueue,
    void *beginItemAddr,
    size_t itemNum,
    size_t itemSize
);
uintptr_t VOS_PriQueTop(const VosPriQue *priQueue);
void VOS_PriQuePop(VosPriQue *priQueue);
bool VOS_PriQueEmpty(const VosPriQue *priQueue);
size_t VOS_PriQueSize(const VosPriQue *priQueue);
void VOS_PriQueClear(VosPriQue *priQueue);
void VOS_PriQueDestroy(VosPriQue *priQueue);

#ifdef __cplusplus
}
#endif

#endif /* VOS_PRIORITYQUEUE_H */
