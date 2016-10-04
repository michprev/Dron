#ifndef __MAIN_H
#define __MAIN_H

#include <stm32f4xx_hal.h>

extern I2C_HandleTypeDef MPU6050_Handle;

#ifdef __cplusplus
extern "C" int get_tick_count(unsigned long *count);
#else
int get_tick_count(unsigned long *count);
#endif
#endif