#include "utils.h"

float absValueFloat(float value) {
    if (value < 0) {
        return -value;
    }
    return value;
}

int countRotationsM3508(float curr_pos, float prev_pos) {
    float threshold = M_ENCODER_M3508_SCALE / 4;

    if (prev_pos > M_ENCODER_M3508_SCALE - threshold && curr_pos < threshold) {
        return 1;
    } else if (prev_pos < threshold && curr_pos > M_ENCODER_M3508_SCALE - threshold) {
        return -1;
    }
    return 0;
}