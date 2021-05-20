#include "utils.h"
#include "constants.h"

float absValueFloat(float value) {
    if (value < 0) {
        return -value;
    }
    return value;
}

int countRotationsM3508(float curr_pos, float prev_pos) {
    float threshold = M_M3508_ENCODER_SCALE / 4;

    if (prev_pos > M_M3508_ENCODER_SCALE - threshold && curr_pos < threshold) {
        return 1;
    } else if (prev_pos < threshold && curr_pos > M_M3508_ENCODER_SCALE - threshold) {
        return -1;
    }
    return 0;
}

bool isM3508AtHardstop(int[M_ZERO_HARDSTOP_TIME_THRESHOLD] encoderHistory) {
    int max = encoderHistory[0];
    int min = encoderHistory[0];
    for (int i = 0; i < M_ZERO_HARDSTOP_TIME_THRESHOLD; i++) {
        int curr = encoderHistory[i];
        if (curr > max)
            max = curr;
        else if (curr < min)
            min = curr;
    }

    float maxRadians = (2 * M_PI * max) / (M_M3508_ENCODER_SCALE);
    float minRadians = (2 * M_PI * max) / (M_M3508_ENCODER_SCALE);
    if ((maxRadians - minRadians) < M_ZERO_HARDSTOP_RANGE_THRESHOLD)
        return true;
    return false;
}