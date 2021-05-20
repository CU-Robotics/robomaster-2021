#include "constants.h"
#include "CAN_receive.h"
#include <cmath>

float prev_pos;
float curr_pos;

float countRotations(float curr_pos, float prev_pos) {
    float threshold = M_MOTOR_M3508_MAX_SPEED + 0.5;

    if (prev_pos > threshold && curr_pos < threshold){
        return 1;
    } else if (prev_pos < threshold && curr_pos > threshold) {
        return -1;
    }
    return 0;  
}


