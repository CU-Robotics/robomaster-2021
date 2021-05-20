#ifndef __utils_H
#define __utils_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "constants.h"

#include <stdlib.h>
#include <stdbool.h>

float absValueFloat(float value);

int countRotationsM3508(float curr_pos, float prev_pos);
bool isM3508AtHardstop(int encoderHistory[M_ZERO_HARDSTOP_TIME_THRESHOLD]);

#ifdef __cplusplus
}
#endif
#endif
