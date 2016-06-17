#ifndef __MPU6050_H
#define __MPU6050_H

#include <stm32f1xx_hal.h>
#include "I2C.h"

class MPU6050 {
private:
	const uint16_t R_SELF_TEST_X = 0x0D;
	const uint16_t R_SELF_TEST_Y = 0x0E;
	const uint16_t R_SELF_TEST_Z = 0x0F;
	const uint16_t R_SELF_TEST_A = 0x10;

	const uint16_t R_SAMPLE_RATE_DIVIDER = 0x19;
	const uint16_t R_CONFIG = 0x1A;
	const uint16_t R_GYRO_CONFIG = 0x1B;
	const uint16_t R_ACCEL_CONFIG = 0x1C;

	const uint16_t R_FIFO_ENABLE = 0x23;
	const uint16_t R_I2C_MASTER_CONTROL = 0x24;
	const uint16_t R_I2C_SLAVE0_ADRRESS = 0x25;
	const uint16_t R_I2C_SLAVE0_REGISTER = 0x26;
	const uint16_t R_I2C_SLAVE0_CONTROL = 0x27;
	const uint16_t R_I2C_SLAVE1_ADRRESS = 0x28;
	const uint16_t R_I2C_SLAVE1_REGISTER = 0x29;
	const uint16_t R_I2C_SLAVE1_CONTROL = 0x2A;
	const uint16_t R_I2C_SLAVE2_ADRRESS = 0x2B;
	const uint16_t R_I2C_SLAVE2_REGISTER = 0x2C;
	const uint16_t R_I2C_SLAVE2_CONTROL = 0x2D;
	const uint16_t R_I2C_SLAVE3_ADRRESS = 0x2E;
	const uint16_t R_I2C_SLAVE3_REGISTER = 0x2F;
	const uint16_t R_I2C_SLAVE3_CONTROL = 0x30;
	const uint16_t R_I2C_SLAVE4_ADRRESS = 0x31;
	const uint16_t R_I2C_SLAVE4_REGISTER = 0x32;
	const uint16_t R_I2C_SLAVE4_DATA_OUTPUT = 0x33;
	const uint16_t R_I2C_SLAVE4_CONTROL = 0x34;
	const uint16_t R_I2C_SLAVE4_DATA_INPUT = 0x35;
	const uint16_t R_I2C_MASTER_STATUS = 0x36;
	const uint16_t R_INT_PIN_CONFIG = 0x37;
	const uint16_t R_INT_ENABLE = 0x38;

	const uint16_t R_INT_STATUS = 0x3A;

	// TODO raw output registers

	const uint16_t R_I2C_SLAVE0_DATA_OUTPUT = 0x63;
	const uint16_t R_I2C_SLAVE1_DATA_OUTPUT = 0x64;
	const uint16_t R_I2C_SLAVE2_DATA_OUTPUT = 0x65;
	const uint16_t R_I2C_SLAVE3_DATA_OUTPUT = 0x66;
	const uint16_t R_I2C_MASTER_DELAY_CONTROL = 0x67;
	const uint16_t R_SIGNAL_PATH_RESET = 0x68;
	const uint16_t R_USER_CONTROL = 0x6A;
	const uint16_t R_POWER_MANAGEMENT_1 = 0x6B;
	const uint16_t R_POWER_MANAGEMENT_2 = 0x6C;

	// TODO FIFO count register

	const uint16_t R_FIFO_RW = 0x74;
	const uint16_t R_WHO_I_AM = 0x75;

	const uint16_t MPU6050_ADDRESS = 0xD0;


	enum GyroRange { GR_250DPS, GR_500DPS, GR_1000DPS, GR_2000DPS };
	enum AccelRange { AR_2G, AR_4G, AR_8G, AR_16G };


	I2C_HandleTypeDef I2C_Handle;

	void GPIO_Init();
	void IT_Init();
	HAL_StatusTypeDef SetGyroRange(GyroRange range);
	HAL_StatusTypeDef SetAccelRange(AccelRange range);
public:
	HAL_StatusTypeDef Init();
};

#endif