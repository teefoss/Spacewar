#ifndef random_h
#define random_h

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

#define RANDOM_INDEX(array) Random(0, ARRAY_SIZE(array))
#define RANDOM_ELEMENT(array) array[RANDOM_INDEX(array)]
#define RANDOM_ANGLE arc4random_uniform(360)

s32 Random(s32 min, s32 max);
float RandomFloat(float min, float max);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* random_h */
