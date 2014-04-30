#include "calc.h"
#include <stdlib.h>

float Calc::random(float low, float high) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = high - low;
    float r = random * diff;
    return low + r;
}
