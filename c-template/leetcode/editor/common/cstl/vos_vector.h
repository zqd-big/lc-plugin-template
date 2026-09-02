#ifndef VOS_VECTOR_H
#define VOS_VECTOR_H

#include <stddef.h>
#include <stdint.h>

#include "vos_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TagVosVector VosVector;

/* Compatibility name requested by the local C exercise set. */
typedef VosVector VOS_VECTOR;

VosVector *VOS_VectorCreate(size_t itemSize);
VosVector *VOS_VectorRawCreate(size_t itemSize, size_t itemCap, uint32_t delta);
uint32_t VOS_VectorPushBack(VosVector *vector, const void *data);
void *VOS_VectorAt(const VosVector *vector, size_t index);
size_t VOS_VectorSize(const VosVector *vector);
uint32_t VOS_VectorErase(VosVector *vector, size_t index);
void VOS_VectorClear(VosVector *vector, VosFreeFunc freeFunc);
void VOS_VectorSort(VosVector *vector, VosDataCmpFunc cmpFunc);
void *VOS_VectorSearch(const VosVector *vector, const void *data, VosDataCmpFunc cmpFunc);
void VOS_VectorDestroy(VosVector *vector, VosFreeFunc freeFunc);

#ifdef __cplusplus
}
#endif

#endif /* VOS_VECTOR_H */
