#include "constants.h"
#include "CAN_receive.h"
#include <cmath>

float prev_pos;
float curr_pos;

//change doubles to float
float position_function(float curr_pos, float prev_pos) {
  
    float threshold = M_MOTOR_M3508_MAX_SPEED + 0.5; //added value subject to change
    float rotation = 2*M_PI;


    if(prev_pos > threshold && curr_pos < threshold){
        
        return curr_pos + rotation;
    }
    
    else if(prev_pos < threshold && curr_pos > threshold) {
       
        return curr_pos - rotation;
        
    else {
        return curr_pos;  
    }
    
    }

    
}


