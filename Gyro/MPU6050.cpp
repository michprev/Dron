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

uint8_t cMPU6050::Init() {
	I2cMaster_Init();
	IT_Init();
	printf("I2C init OK\n");
	int result;
	struct int_param_s int_param;
	unsigned char accel_fsr, new_temp = 0;
	unsigned short gyro_rate, gyro_fsr;

	if (result = mpu_init(&int_param))
		return 1;

	if (result = inv_init_mpl())
		return 2;

	/* Compute 6-axis and 9-axis quaternions. */
	if (inv_enable_quaternion())
		return 3;
	if (inv_enable_9x_sensor_fusion())
		return 4;

	/* Update gyro biases when not in motion.
	* WARNING: These algorithms are mutually exclusive.
	*/
	if (inv_enable_fast_nomot())
		return 5;
	/* inv_enable_motion_no_motion(); */
	/* inv_set_no_motion_time(1000); */

	/* Update gyro biases when temperature changes. */
	if (inv_enable_gyro_tc())
		return 6;

	/* Allows use of the MPL APIs in read_from_mpl. */
	if (inv_enable_eMPL_outputs())
		return 7;

	if (result = inv_start_mpl())
		return 8;

	if (mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL) || mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL) || mpu_set_sample_rate(20))
		return 9;

	/* Read back configuration in case it was set improperly. */
	if (mpu_get_sample_rate(&gyro_rate) || mpu_get_gyro_fsr(&gyro_fsr) || mpu_get_accel_fsr(&accel_fsr))
		return 10;

	/* Sync driver configuration with MPL. */
	/* Sample rate expected in microseconds. */
	inv_set_gyro_sample_rate(1000000L / gyro_rate);
	inv_set_accel_sample_rate(1000000L / gyro_rate);

	/* Set chip-to-body orientation matrix.
	* Set hardware units to dps/g's/degrees scaling factor.
	*/
	inv_set_gyro_orientation_and_scale(
		inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
		(long)gyro_fsr << 15);
	inv_set_accel_orientation_and_scale(
		inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
		(long)accel_fsr << 15);

	if (result = dmp_load_motion_driver_firmware())
		return 11;
	if (dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_pdata.orientation)))
		return 12;

	if (dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP | DMP_FEATURE_SEND_RAW_GYRO | DMP_FEATURE_SEND_RAW_ACCEL))
		return 13;
	if (dmp_set_fifo_rate(20))
		return 14;
	if (mpu_set_dmp_state(1))
		return 15;


	return 0;
}

extern "C" void EXTI4_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}