#include "vos_vector.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define VOS_VECTOR_DEFAULT_CAPACITY ((size_t)2)

struct TagVosVector {
    unsigned char *data;
    size_t itemSize;
    size_t size;
    size_t capacity;
    size_t initialCapacity;
    uint32_t delta;
};

static int VosVectorCanAllocate(const VosVector *vector, size_t capacity)
{
    return vector != NULL && vector->itemSize != 0 &&
        capacity <= SIZE_MAX / vector->itemSize;
}

static size_t VosVectorNextCapacity(const VosVector *vector, size_t current)
{
    if (vector->delta != 0u) {
        if (current > SIZE_MAX - (size_t)vector->delta) {
            return 0;
        }
        return current + (size_t)vector->delta;
    }
    if (current > SIZE_MAX / 2u) {
        return 0;
    }
    return current * 2u;
}

static uint32_t VosVectorReserve(VosVector *vector, size_t required)
{
    size_t capacity;
    unsigned char *newData;

    if (vector == NULL) {
        return VOS_ERROR;
    }
    if (required <= vector->capacity) {
        return VOS_OK;
    }

    capacity = vector->capacity;
    if (capacity == 0) {
        capacity = vector->initialCapacity != 0
            ? vector->initialCapacity
            : VOS_VECTOR_DEFAULT_CAPACITY;
    }
    while (capacity < required) {
        capacity = VosVectorNextCapacity(vector, capacity);
        if (capacity == 0) {
            return VOS_ERROR;
        }
    }
    if (!VosVectorCanAllocate(vector, capacity)) {
        return VOS_ERROR;
    }

    newData = (unsigned char *)realloc(vector->data, capacity * vector->itemSize);
    if (newData == NULL) {
        return VOS_ERROR;
    }
    vector->data = newData;
    vector->capacity = capacity;
    return VOS_OK;
}

VosVector *VOS_VectorRawCreate(size_t itemSize, size_t itemCap, uint32_t delta)
{
    VosVector *vector;
    size_t capacity = itemCap == 0 ? VOS_VECTOR_DEFAULT_CAPACITY : itemCap;

    if (itemSize == 0 || capacity > SIZE_MAX / itemSize) {
        return NULL;
    }

    vector = (VosVector *)calloc(1, sizeof(*vector));
    if (vector == NULL) {
        return NULL;
    }
    vector->data = (unsigned char *)calloc(capacity, itemSize);
    if (vector->data == NULL) {
        free(vector);
        return NULL;
    }
    vector->itemSize = itemSize;
    vector->capacity = capacity;
    vector->initialCapacity = capacity;
    vector->delta = delta;
    return vector;
}

VosVector *VOS_VectorCreate(size_t itemSize)
{
    return VOS_VectorRawCreate(itemSize, VOS_VECTOR_DEFAULT_CAPACITY, 0u);
}

uint32_t VOS_VectorPushBack(VosVector *vector, const void *data)
{
    const unsigned char *source = (const unsigned char *)data;
    size_t sourceOffset = 0;
    size_t usedBytes;
    int sourceIsInternal = 0;

    if (vector == NULL || data == NULL || vector->size == SIZE_MAX) {
        return VOS_ERROR;
    }

    if (vector->data != NULL && vector->size != 0u) {
        uintptr_t sourceAddress = (uintptr_t)source;
        uintptr_t beginAddress = (uintptr_t)vector->data;
        usedBytes = vector->size * vector->itemSize;
        if (sourceAddress >= beginAddress) {
            sourceOffset = (size_t)(sourceAddress - beginAddress);
            if (sourceOffset <= usedBytes - vector->itemSize &&
                sourceOffset % vector->itemSize == 0u) {
                sourceIsInternal = 1;
            }
        }
    }

    if (VosVectorReserve(vector, vector->size + 1u) != VOS_OK) {
        return VOS_ERROR;
    }
    if (sourceIsInternal) {
        source = vector->data + sourceOffset;
    }
    memmove(
        vector->data + vector->size * vector->itemSize,
        source,
        vector->itemSize
    );
    vector->size += 1u;
    return VOS_OK;
}

void *VOS_VectorAt(const VosVector *vector, size_t index)
{
    if (vector == NULL || index >= vector->size) {
        return NULL;
    }
    return vector->data + index * vector->itemSize;
}

size_t VOS_VectorSize(const VosVector *vector)
{
    return vector == NULL ? 0u : vector->size;
}

uint32_t VOS_VectorErase(VosVector *vector, size_t index)
{
    unsigned char *position;

    if (vector == NULL || index >= vector->size) {
        return VOS_ERROR;
    }
    position = vector->data + index * vector->itemSize;
    if (index + 1u < vector->size) {
        memmove(
            position,
            position + vector->itemSize,
            (vector->size - index - 1u) * vector->itemSize
        );
    }
    vector->size -= 1u;
    memset(vector->data + vector->size * vector->itemSize, 0, vector->itemSize);
    return VOS_OK;
}

void VOS_VectorClear(VosVector *vector, VosFreeFunc freeFunc)
{
    size_t index;
    unsigned char *newData = NULL;

    if (vector == NULL) {
        return;
    }
    if (freeFunc != NULL) {
        for (index = 0; index < vector->size; ++index) {
            freeFunc(vector->data + index * vector->itemSize);
        }
    }
    free(vector->data);
    if (VosVectorCanAllocate(vector, vector->initialCapacity)) {
        newData = (unsigned char *)calloc(vector->initialCapacity, vector->itemSize);
    }
    vector->data = newData;
    vector->size = 0;
    vector->capacity = newData == NULL ? 0u : vector->initialCapacity;
}

void VOS_VectorSort(VosVector *vector, VosDataCmpFunc cmpFunc)
{
    if (vector == NULL || cmpFunc == NULL || vector->size < 2u) {
        return;
    }
    qsort(vector->data, vector->size, vector->itemSize, cmpFunc);
}

void *VOS_VectorSearch(const VosVector *vector, const void *data, VosDataCmpFunc cmpFunc)
{
    if (vector == NULL || data == NULL || cmpFunc == NULL || vector->size == 0) {
        return NULL;
    }
    return bsearch(data, vector->data, vector->size, vector->itemSize, cmpFunc);
}

void VOS_VectorDestroy(VosVector *vector, VosFreeFunc freeFunc)
{
    size_t index;

    if (vector == NULL) {
        return;
    }
    if (freeFunc != NULL) {
        for (index = 0; index < vector->size; ++index) {
            freeFunc(vector->data + index * vector->itemSize);
        }
    }
    free(vector->data);
    free(vector);
}
