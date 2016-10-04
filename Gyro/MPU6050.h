#ifndef __MPU6050_H
#define __MPU6050_H

extern "C" {

#include <stm32f4xx_hal.h>
#include "main.h"
#include "invensense\inv_mpu.h"
#include "invensense\inv_mpu_dmp_motion_driver.h"
#include "invensense\mltypes.h"
#include "invensense\mpl.h"
#include "invensense\library\invensense_adv.h"
#include "invensense\eMPL_outputs.h"
#include "invensense\invensense.h"
#include "invensense\i2c.h"

}

class cMPU6050 {
private:
	struct platform_data_s {
		signed char orientation[9];
	};
	struct platform_data_s gyro_pdata = {
		{ 1, 0, 0,
		0, 1, 0,
		0, 0, 1 }
	};
	unsigned long next_temp_ms = 0;
	bool new_temp = false;
	unsigned long timestamp;

	void IT_Init();

public:
	bool dataReady;

	uint8_t Init();
	bool CheckNewData(long *euler, uint8_t *accur);
};

#endif