#ifndef __time_manip_H
#define __time_manip_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

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
int function_with_interval(void (*function)(int*), int* parameters, int interval);

#ifdef __cplusplus
}
#endif
#endif
