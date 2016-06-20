#include "MPU6050.h"

void cMPU6050::IT_Init() {
	if (__GPIOA_IS_CLK_DISABLED())
		__GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef exti;

	exti.Pin = GPIO_PIN_4;
	exti.Mode = GPIO_MODE_IT_RISING;
	exti.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &exti);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

HAL_StatusTypeDef cMPU6050::Init() {
	I2cMaster_Init();
	IT_Init();

	int result;
	struct int_param_s int_param;
	unsigned char accel_fsr, new_temp = 0;
	unsigned short gyro_rate, gyro_fsr;

	if (result = mpu_init(&int_param))
		return HAL_ERROR;

	if (result = inv_init_mpl())
		return HAL_ERROR;

	/* Compute 6-axis and 9-axis quaternions. */
	inv_enable_quaternion();
	inv_enable_9x_sensor_fusion();

	/* Update gyro biases when not in motion.
	* WARNING: These algorithms are mutually exclusive.
	*/
	inv_enable_fast_nomot();
	/* inv_enable_motion_no_motion(); */
	/* inv_set_no_motion_time(1000); */

	/* Update gyro biases when temperature changes. */
	inv_enable_gyro_tc();

	/* Allows use of the MPL APIs in read_from_mpl. */
	inv_enable_eMPL_outputs();

	if (result = inv_start_mpl())
		return HAL_ERROR;

	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	mpu_set_sample_rate(20);

	/* Read back configuration in case it was set improperly. */
	mpu_get_sample_rate(&gyro_rate);
	mpu_get_gyro_fsr(&gyro_fsr);
	mpu_get_accel_fsr(&accel_fsr);

	/* Sync driver configuration with MPL. */
	/* Sample rate expected in microseconds. */
	inv_set_gyro_sample_rate(1000000L / gyro_rate);
	inv_set_accel_sample_rate(1000000L / gyro_rate);
}

extern "C" void EXTI4_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}