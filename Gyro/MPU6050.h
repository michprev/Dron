#ifndef __MPU6050_H
#define __MPU6050_H


#include <stm32f4xx_hal.h>

extern "C" {

#include "invensense\inv_mpu.h"
#include "invensense\inv_mpu_dmp_motion_driver.h"
#include "invensense\mltypes.h"
#include "invensense\mpl.h"
#include "invensense\library\invensense_adv.h"
#include "invensense\eMPL_outputs.h"
#include "invensense\invensense.h"
#include "invensense\i2c.h"

}

class MPU6050 {
private:
	struct platform_data_s {
		signed char orientation[9];
	};
	struct platform_data_s gyro_pdata = {
		{ 0, -1, 0,
		1, 0, 0,
		0, 0, 1 }
	};
	struct platform_data_s compass_pdata = {
		{ 0, -1, 0,
		1, 0, 0,
		0, 0, 1 }
	};
	const uint8_t COMPASS_READ_MS = 100;
	unsigned long next_temp_ms = 0;
	unsigned long next_compass_ms = 0;
	bool new_temp = false;
	unsigned long timestamp;

	void IT_Init();
	uint32_t DELAY_Init(void);

public:
	bool dataReady;

	uint8_t Init();
	void selfTest();
	bool CheckNewData(long *euler, uint8_t *accur);
};

#endif