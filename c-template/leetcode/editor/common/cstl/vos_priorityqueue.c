#include "vos_priorityqueue.h"

#include <stdlib.h>

struct tagVosPriQue {
    uintptr_t *data;
    size_t size;
    size_t capacity;
    VosPriQueCmpFunc cmpFunc;
    VosDupFreeFuncPair dataFunc;
};

static int VosPriQueHigher(const VosPriQue *priQueue, uintptr_t left, uintptr_t right)
{
    if (priQueue->cmpFunc != NULL) {
        return priQueue->cmpFunc(left, right) > 0;
    }
    return left > right;
}

static uint32_t VosPriQueReserve(VosPriQue *priQueue, size_t required)
{
    size_t capacity;
    uintptr_t *newData;

    if (required <= priQueue->capacity) {
        return VOS_OK;
    }
    capacity = priQueue->capacity == 0 ? 8u : priQueue->capacity;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2u) {
            return VOS_ERROR;
        }
        capacity *= 2u;
    }
    if (capacity > SIZE_MAX / sizeof(*newData)) {
        return VOS_ERROR;
    }
    newData = (uintptr_t *)realloc(priQueue->data, capacity * sizeof(*newData));
    if (newData == NULL) {
        return VOS_ERROR;
    }
    priQueue->data = newData;
    priQueue->capacity = capacity;
    return VOS_OK;
}

static void VosPriQueSwap(uintptr_t *left, uintptr_t *right)
{
    uintptr_t temporary = *left;
    *left = *right;
    *right = temporary;
}

static void VosPriQuePushOwned(VosPriQue *priQueue, uintptr_t value)
{
    size_t index = priQueue->size;

    priQueue->data[index] = value;
    priQueue->size += 1u;
    while (index > 0u) {
        size_t parent = (index - 1u) / 2u;
        if (!VosPriQueHigher(priQueue, priQueue->data[index], priQueue->data[parent])) {
            break;
        }
        VosPriQueSwap(&priQueue->data[index], &priQueue->data[parent]);
        index = parent;
    }
}

static void VosPriQueRelease(VosPriQue *priQueue, uintptr_t value)
{
    if (priQueue->dataFunc.freeFunc != NULL) {
        priQueue->dataFunc.freeFunc((void *)value);
    }
}

VosPriQue *VOS_PriQueCreate(VosPriQueCmpFunc cmpFunc, VosDupFreeFuncPair *dataFunc)
{
    VosPriQue *priQueue = (VosPriQue *)calloc(1, sizeof(*priQueue));

    if (priQueue == NULL) {
        return NULL;
    }
    priQueue->cmpFunc = cmpFunc;
    if (dataFunc != NULL) {
        priQueue->dataFunc = *dataFunc;
    }
    return priQueue;
}

uint32_t VOS_PriQuePush(VosPriQue *priQueue, uintptr_t value)
{
    uintptr_t storedValue = value;
    int duplicated = 0;

    if (priQueue == NULL || priQueue->size == SIZE_MAX) {
        return VOS_ERROR;
    }
    if (priQueue->dataFunc.dupFunc != NULL) {
        storedValue = (uintptr_t)priQueue->dataFunc.dupFunc((void *)value);
        if (storedValue == (uintptr_t)0) {
            return VOS_ERROR;
        }
        duplicated = 1;
    }
    if (VosPriQueReserve(priQueue, priQueue->size + 1u) != VOS_OK) {
        if (duplicated) {
            VosPriQueRelease(priQueue, storedValue);
        }
        return VOS_ERROR;
    }
    VosPriQuePushOwned(priQueue, storedValue);
    return VOS_OK;
}

uint32_t VOS_PriQuePushBatch(
    VosPriQue *priQueue,
    void *beginItemAddr,
    size_t itemNum,
    size_t itemSize
)
{
    uintptr_t *staged;
    size_t index;

    if (priQueue == NULL || itemSize == 0u) {
        return VOS_ERROR;
    }
    if (itemNum == 0u) {
        return VOS_OK;
    }
    if (beginItemAddr == NULL || priQueue->dataFunc.dupFunc == NULL ||
        itemNum > SIZE_MAX / itemSize ||
        itemNum > SIZE_MAX / sizeof(*staged) ||
        priQueue->size > SIZE_MAX - itemNum) {
        return VOS_ERROR;
    }

    staged = (uintptr_t *)calloc(itemNum, sizeof(*staged));
    if (staged == NULL) {
        return VOS_ERROR;
    }
    for (index = 0; index < itemNum; ++index) {
        void *source = (unsigned char *)beginItemAddr + index * itemSize;
        staged[index] = (uintptr_t)priQueue->dataFunc.dupFunc(source);
        if (staged[index] == (uintptr_t)0) {
            size_t rollback;
            for (rollback = 0; rollback < index; ++rollback) {
                VosPriQueRelease(priQueue, staged[rollback]);
            }
            free(staged);
            return VOS_ERROR;
        }
    }
    if (VosPriQueReserve(priQueue, priQueue->size + itemNum) != VOS_OK) {
        for (index = 0; index < itemNum; ++index) {
            VosPriQueRelease(priQueue, staged[index]);
        }
        free(staged);
        return VOS_ERROR;
    }
    for (index = 0; index < itemNum; ++index) {
        VosPriQuePushOwned(priQueue, staged[index]);
    }
    free(staged);
    return VOS_OK;
}

uintptr_t VOS_PriQueTop(const VosPriQue *priQueue)
{
    if (priQueue == NULL || priQueue->size == 0u) {
        return (uintptr_t)0;
    }
    return priQueue->data[0];
}

void VOS_PriQuePop(VosPriQue *priQueue)
{
    size_t index;

    if (priQueue == NULL || priQueue->size == 0u) {
        return;
    }
    VosPriQueRelease(priQueue, priQueue->data[0]);
    priQueue->size -= 1u;
    if (priQueue->size == 0u) {
        return;
    }
    priQueue->data[0] = priQueue->data[priQueue->size];
    index = 0u;
    for (;;) {
        size_t left = index * 2u + 1u;
        size_t right = left + 1u;
        size_t highest = index;

        if (left < priQueue->size &&
            VosPriQueHigher(priQueue, priQueue->data[left], priQueue->data[highest])) {
            highest = left;
        }
        if (right < priQueue->size &&
            VosPriQueHigher(priQueue, priQueue->data[right], priQueue->data[highest])) {
            highest = right;
        }
        if (highest == index) {
            break;
        }
        VosPriQueSwap(&priQueue->data[index], &priQueue->data[highest]);
        index = highest;
    }
}

bool VOS_PriQueEmpty(const VosPriQue *priQueue)
{
    return priQueue == NULL || priQueue->size == 0u;
}

size_t VOS_PriQueSize(const VosPriQue *priQueue)
{
    return priQueue == NULL ? 0u : priQueue->size;
}

void VOS_PriQueClear(VosPriQue *priQueue)
{
    size_t index;

    if (priQueue == NULL) {
        return;
    }
    for (index = 0; index < priQueue->size; ++index) {
        VosPriQueRelease(priQueue, priQueue->data[index]);
    }
    free(priQueue->data);
    priQueue->data = NULL;
    priQueue->size = 0u;
    priQueue->capacity = 0u;
}

void VOS_PriQueDestroy(VosPriQue *priQueue)
{
    if (priQueue == NULL) {
        return;
    }
    VOS_PriQueClear(priQueue);
    free(priQueue);
}
