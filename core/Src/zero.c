#include "CAN_receive.c"
#include "zero.h"
#include <cmath>

double max_function(double get_yaw_gimbal_motor_measure_point()->ecd) {
    motor.go(1000) // subject to change
    int array[3];

    while(1) {
        bool match = true; 
        int current_pos = (M_PI * get_yaw_gimbal_motor_measure_point()->ecd) / 180.0;

        // if the current position is within the range 1 - 5, set the limit (lim_1) equal to current position.
        for (int i=0; i<3;i++) {
            if (abs(array[i] - current_pos) < 5 && abs(array[i] - current_pos) > 1) {
                lim_1 = current_pos;
            } else {
                match = false;
            } 
        }
        array[2] = array[1];
        array[1] = array[0];
        array[0] = current_pos; // scoot the array over by 1 value so current position is in place of first element of array.
    }

    return lim_1;
}

double min_function(double get_yaw_gimbal_motor_measure_point()->ecd){
    motor.go(-1000)
    int array[3];

    while(1) {
        bool match = true; 
        int current_pos = (M_PI* get_yaw_gimbal_motor_measure_point()->ecd) / 180.0;

        for (int i=1; i<4;i++) {
            if (abs(array[i] - current_pos) < 5 && abs(array[i] - current_pos) > 1) {
                lim_2 = current_pos;
            } else {
                match = false;
            } 
        }

        array[2] = array[1];
        array[1] = array[0];
        array[0] = current_pos;
    }

    return lim_2;
}



