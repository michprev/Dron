#include <stm32f4xx_hal.h>
#include "main.h"
#include "MPU6050.h"

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

extern "C" int get_tick_count(unsigned long *count)
{
	count[0] = HAL_GetTick();
	return 0;
}

void GPIO_Init();

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";
bool dataReady = false;

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	dataReady = true;
}

int main(void)
{
	HAL_Init();
	cMPU6050 mpu;
	__GPIOA_CLK_ENABLE();
	
	GPIO_Init();

	uint8_t result;
	printf("Started\n");
	if (result = mpu.Init()) {
		printf("Error %d\n", result);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

	}
	else
		printf("Everything ok..\n");
	//I2cMaster_Init();

	//uint8_t tmp = 0b10000000;
	//HAL_I2C_Mem_Write(&MPU6050_Handle, 0x68 << 1, 0x6B, I2C_MEMADD_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY);
	//HAL_Delay(50);
	//tmp = 0;
	//HAL_I2C_Mem_Write(&MPU6050_Handle, 0x68 << 1, 0x6B, I2C_MEMADD_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY);

	//HAL_Delay(30);

	//uint8_t writeBuff[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	//uint8_t readBuff[16];

	//for (int p = 0; p < 11; p++) {
	//	for (int i = 0; i < 256; i += 16) {
	//		tmp = p;
	//		HAL_I2C_Mem_Write(&MPU6050_Handle, 0x68 << 1, 0x6D, I2C_MEMADD_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY);
	//		tmp = i;
	//		HAL_I2C_Mem_Write(&MPU6050_Handle, 0x68 << 1, 0x6E, I2C_MEMADD_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY);
	//		HAL_I2C_Mem_Write(&MPU6050_Handle, 0x68 << 1, 0x6F, I2C_MEMADD_SIZE_8BIT, writeBuff, 16, HAL_MAX_DELAY);


	//		tmp = p;
	//		HAL_I2C_Mem_Write(&MPU6050_Handle, 0x68 << 1, 0x6D, I2C_MEMADD_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY);
	//		tmp = i;
	//		HAL_I2C_Mem_Write(&MPU6050_Handle, 0x68 << 1, 0x6E, I2C_MEMADD_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY);

	//		HAL_I2C_Mem_Read(&MPU6050_Handle, 0x68 << 1, 0x6F, I2C_MEMADD_SIZE_8BIT, readBuff, 16, HAL_MAX_DELAY);

	//		for (int j = 0; j < 16; j++) {
	//			if (memcmp(readBuff, writeBuff, 16)) {
	//				uint8_t h = i + j;
	//				uint8_t l = p;
	//				HAL_UART_Transmit(&UART1_Handle, &l, 1, HAL_MAX_DELAY);
	//				HAL_UART_Transmit(&UART1_Handle, &h, 1, HAL_MAX_DELAY);
	//				HAL_UART_Transmit(&UART1_Handle, readBuff + j, 1, HAL_MAX_DELAY);
	//			}
	//		}
	//	}
	//}
	
	uint t = 0;
	unsigned long next_temp_ms = 0;
	unsigned long next_compass_ms = 0;
	bool new_temp = false;
	bool new_compass = false;
	unsigned long timestamp;

	while (true)
	{
		bool new_data = false;
		unsigned long sensor_timestamp;
		get_tick_count(&timestamp);

		if ((timestamp > next_compass_ms) && dataReady) {
			next_compass_ms = timestamp + 100;
			new_compass = 1;
		}

		if (timestamp > next_temp_ms) {
			next_temp_ms = timestamp + 500;
			new_temp = true;
		}

		if (dataReady) {
			short gyro[3], accel_short[3], sensors;
			unsigned char more;
			long accel[3], quat[4], temperature;

			dmp_read_fifo(gyro, accel_short, quat, &sensor_timestamp, &sensors, &more);

			if (!more)
				dataReady = false;
			if (sensors & INV_XYZ_GYRO) {
				/* Push the new data to the MPL. */
				inv_build_gyro(gyro, sensor_timestamp);
				new_data = true;
				if (new_temp) {
					new_temp = false;
					/* Temperature only used for gyro temp comp. */
					mpu_get_temperature(&temperature, &sensor_timestamp);
					inv_build_temp(temperature, sensor_timestamp);
				}
			}
			if (sensors & INV_XYZ_ACCEL) {
				accel[0] = (long)accel_short[0];
				accel[1] = (long)accel_short[1];
				accel[2] = (long)accel_short[2];
				inv_build_accel(accel, 0, sensor_timestamp);
				new_data = true;
			}
			if (sensors & INV_WXYZ_QUAT) {
				inv_build_quat(quat, 0, sensor_timestamp);
				new_data = true;
			}
		}
		if (new_compass) {
			short compass_short[3];
			long compass[3];
			new_compass = 0;
			/* For any MPU device with an AKM on the auxiliary I2C bus, the raw
			* magnetometer registers are copied to special gyro registers.
			*/
			if (!mpu_get_compass_reg(compass_short, &sensor_timestamp)) {
				compass[0] = (long)compass_short[0];
				compass[1] = (long)compass_short[1];
				compass[2] = (long)compass_short[2];
				/* NOTE: If using a third-party compass calibration library,
				* pass in the compass data in uT * 2^16 and set the second
				* parameter to INV_CALIBRATED | acc, where acc is the
				* accuracy from 0 to 3.
				*/
				inv_build_compass(compass, 0, sensor_timestamp);
			}
			new_data = 1;
		}

		if (new_data) {
			inv_execute_on_data();

			long msg, data[9];
			int8_t accuracy;
			unsigned long timestamp;
			float float_data[3] = { 0 };

			/*if (inv_get_sensor_type_quat(data, &accuracy, (inv_time_t*)&timestamp))
				printf("Data: %d %d %d %d %d %d %d %d %d", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);*/

			if (inv_get_sensor_type_euler(data, &accuracy, (inv_time_t*)&timestamp))
				printf("Data: %d %d %d\n", data[0] / 65536, data[1] / 65536, data[2] / 65536);
		}

		if (t == 100000) {
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			t = 0;
		}
		t++;
	}
}

void GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_5;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}
