#include "MPU9250.h"

void MPU9250::IT_Init() {
	if (__GPIOB_IS_CLK_DISABLED())
		__GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef exti;

	exti.Pin = GPIO_PIN_1;
	exti.Mode = GPIO_MODE_IT_RISING;
	exti.Pull = GPIO_NOPULL;
	exti.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &exti);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

void MPU9250::selfTest()
{
	int result;
	long gyro[3], accel[3];

	result = mpu_run_self_test(gyro, accel);

	if (result == 0x7) {
		printf("Passed!\n");
		printf("accel: %7.4f %7.4f %7.4f\n",
			accel[0] / 65536.f,
			accel[1] / 65536.f,
			accel[2] / 65536.f);
		printf("gyro: %7.4f %7.4f %7.4f\n",
			gyro[0] / 65536.f,
			gyro[1] / 65536.f,
			gyro[2] / 65536.f);

		unsigned short accel_sens;
		float gyro_sens;

		mpu_get_accel_sens(&accel_sens);
		accel[0] *= accel_sens;
		accel[1] *= accel_sens;
		accel[2] *= accel_sens;
		inv_set_accel_bias(accel, 3);
		mpu_get_gyro_sens(&gyro_sens);
		gyro[0] = (long)(gyro[0] * gyro_sens);
		gyro[1] = (long)(gyro[1] * gyro_sens);
		gyro[2] = (long)(gyro[2] * gyro_sens);
		inv_set_gyro_bias(gyro, 3);
	}
	else {
		if (!(result & 0x1))
			printf("Gyro failed.\n");
		if (!(result & 0x2))
			printf("Accel failed.\n");
		if (!(result & 0x4))
			printf("Compass failed.\n");
	}
}

uint8_t MPU9250::Init() {
	I2cMaster_Init();
	IT_Init();
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

	if (inv_enable_vector_compass_cal())
		return 7;
	if (inv_enable_magnetic_disturbance())
		return 8;

	/* Allows use of the MPL APIs in read_from_mpl. */
	if (inv_enable_eMPL_outputs())
		return 9;

	if (inv_start_mpl())
		return 10;

	if (mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS) || mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL) || mpu_set_sample_rate(this->GYRO_SAMPLE_RATE) || mpu_set_compass_sample_rate(10))
		return 11;
	/* Read back configuration in case it was set improperly. */
	if (mpu_get_sample_rate(&gyro_rate) || mpu_get_gyro_fsr(&gyro_fsr) || mpu_get_accel_fsr(&accel_fsr) || mpu_get_compass_fsr(&compass_fsr))
		return 12;

	/* Sync driver configuration with MPL. */
	/* Sample rate expected in microseconds. */
	inv_set_gyro_sample_rate(1000000L / gyro_rate);
	inv_set_accel_sample_rate(1000000L / gyro_rate);

	inv_set_compass_sample_rate(100 * 1000L);

	/* Set chip-to-body orientation matrix.
	* Set hardware units to dps/g's/degrees scaling factor.
	*/
	inv_set_gyro_orientation_and_scale(
		inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
		(long)gyro_fsr << 15);
	inv_set_accel_orientation_and_scale(
		inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
		(long)accel_fsr << 15);

	inv_set_compass_orientation_and_scale(
		inv_orientation_matrix_to_scalar(compass_pdata.orientation),
		(long)compass_fsr << 15);

	if (dmp_load_motion_driver_firmware())
		return 13;
	if (dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_pdata.orientation)))
		return 14;

	if (dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP | DMP_FEATURE_SEND_RAW_GYRO | DMP_FEATURE_SEND_RAW_ACCEL))
		return 15;
	if (dmp_set_fifo_rate(this->GYRO_SAMPLE_RATE))
		return 16;
	if (mpu_set_dmp_state(1))
		return 17;

	selfTest();

	return 0;
}

bool MPU9250::CheckNewData(long *euler, uint8_t *accur)
{
	bool new_data = false;
	bool new_compass = false;
	unsigned long sensor_timestamp;

	timestamp = HAL_GetTick();

	if ((timestamp > this->next_compass_ms) /*&& !hal.lp_accel_mode*/ &&
		dataReady /*&& (hal.sensors & COMPASS_ON)*/) {
		this->next_compass_ms = timestamp + COMPASS_READ_MS;
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

			//printf("compass %d %d %d\n", compass[0], compass[1], compass[2]);

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

		//if (inv_get_sensor_type_heading(data, &accuracy, (inv_time_t*)&timestamp)){}
		//printf("heading %d accur %d\n", data[0] / 65536, accuracy);

		/*if (inv_get_sensor_type_compass(data, &accuracy, (inv_time_t*)&timestamp))
		printf("compass %d %d %d\n", data[0], data[1], data[2]);*/

		if (inv_get_sensor_type_euler(data, &accuracy, (inv_time_t*)&timestamp)) {
			euler[0] = data[0] / 65536;
			euler[1] = data[1] / 65536;
			euler[2] = data[2] / 65536;
			(*accur) = accuracy;
		}

		return true;
	}

	return false;
}

#ifdef MPU9250
extern "C" void EXTI1_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
#endif // MPU9250