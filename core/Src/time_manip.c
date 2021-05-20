#include "time_manip.h"

/* function with interval
 * @breif: Controls the time interval that a function takes to run
 * @param: 
   * the function to be run
   * array of intagers required to run the function
   * the interval in milliseconds to run it at
 * @returns:
   *  0 for success
   * -1 when function took longer than the interval to run
 * @Notes 
   * If you are having problems with HAL_GetTick you might have to put a "void" in there
   * This is a ms accurate timer but a different implimentation would be required for more accuracy
 */
int function_with_interval(void (*function)(int*), int* parameters, int interval) {
    uint32_t startTick;                     //starting time
    uint32_t endTick;                       //current time
    startTick = HAL_GetTick();              //get start time
    (*function)(parameters);                //run function
    endTick = HAL_GetTick();                //get end time
    if (endTick-startTick > interval) {      //if the time it took to run the function is greater than desired interval
        return -1;                            //return error -1
    } do {                                    
        endTick = HAL_GetTick();              //update the current time
    } while(endTick-startTick < interval);  //until desired time interval has been reached
    return 0;                               //return success
}