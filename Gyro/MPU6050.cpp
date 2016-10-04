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
	struct int_param_s int_param;
	unsigned char accel_fsr, new_temp = 0;
	unsigned short gyro_rate, gyro_fsr, compass_fsr;

	if (mpu_init(&int_param))
		return 1;

	if (inv_init_mpl())
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
#ifdef COMPASS_ENABLED
	if (inv_enable_vector_compass_cal())
		return 7;
	if (inv_enable_magnetic_disturbance())
		return 8;
#endif
	/* Allows use of the MPL APIs in read_from_mpl. */
	if (inv_enable_eMPL_outputs())
		return 9;

	if (inv_start_mpl())
		return 10;
#ifdef COMPASS_ENABLED
	if (mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS) || mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL) || mpu_set_sample_rate(20) || mpu_set_compass_sample_rate(10))
		return 11;
	/* Read back configuration in case it was set improperly. */
	if (mpu_get_sample_rate(&gyro_rate) || mpu_get_gyro_fsr(&gyro_fsr) || mpu_get_accel_fsr(&accel_fsr) || mpu_get_compass_fsr(&compass_fsr))
		return 12;
#else
	if (mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL) || mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL) || mpu_set_sample_rate(20))
		return 11;
	/* Read back configuration in case it was set improperly. */
	if (mpu_get_sample_rate(&gyro_rate) || mpu_get_gyro_fsr(&gyro_fsr) || mpu_get_accel_fsr(&accel_fsr))
		return 12;
#endif

	/* Sync driver configuration with MPL. */
	/* Sample rate expected in microseconds. */
	inv_set_gyro_sample_rate(1000000L / gyro_rate);
	inv_set_accel_sample_rate(1000000L / gyro_rate);
#ifdef COMPASS_ENABLED
	inv_set_compass_sample_rate(100 * 1000L);
#endif
	/* Set chip-to-body orientation matrix.
	* Set hardware units to dps/g's/degrees scaling factor.
	*/
	inv_set_gyro_orientation_and_scale(
		inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
		(long)gyro_fsr << 15);
	inv_set_accel_orientation_and_scale(
		inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
		(long)accel_fsr << 15);

	if (dmp_load_motion_driver_firmware())
		return 13;
	if (dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_pdata.orientation)))
		return 14;

	if (dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP | DMP_FEATURE_SEND_RAW_GYRO | DMP_FEATURE_SEND_RAW_ACCEL))
		return 15;
	if (dmp_set_fifo_rate(20))
		return 16;
	if (mpu_set_dmp_state(1))
		return 17;


	return 0;
}

bool cMPU6050::CheckNewData(long *euler, uint8_t *accur)
{
	bool new_data = false;
	unsigned long sensor_timestamp;
	get_tick_count(&timestamp);

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

	if (new_data) {
		inv_execute_on_data();

		long msg, data[9];
		int8_t accuracy;
		unsigned long timestamp;

		if (inv_get_sensor_type_euler(data, &accuracy, (inv_time_t*)&timestamp) && accuracy != 0)
			printf("Data: %d %d %d, accuracy: %d\n", data[0] / 65536, data[1] / 65536, data[2] / 65536, accuracy);
	}
}

extern "C" void EXTI4_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}