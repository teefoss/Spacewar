#include "random.h"
#include <stdlib.h>


s32 Random(s32 min, s32 max)
{
    return arc4random_uniform(max - min) + min;
}


float RandomFloat(float min, float max)
{
    float random = (float)arc4random() / (float)U32_MAX;
    return random * (max - min) + min;
}
