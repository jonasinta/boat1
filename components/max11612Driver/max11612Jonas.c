//#include <driver/i2c.h>
//#include <esp_log.h>
//#include <freertos/FreeRTOS.h>
//#include <freertos/task.h>

#include "include/max11612Jonas.h"

static const char* tag = "MAX11612Jonas";
int public_heap = 0; // project variables
double public_tempC = 0;

void init_max11612Jonas(void) {
	// initialise the i2c...................
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = SDA_IOpin;
	conf.scl_io_num = SCL_IOpin;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 400000;//clock speed
	i2c_param_config(I2C_NUM_0, &conf);
	i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

//..................................................................
	uint8_t setup=0;
	uint8_t config=0;
	esp_log_level_set("max11612", ESP_LOG_DEBUG);
	//Reset device
	setup = (Max11612reset);
	send_byte2config_setup(setup, config);
	setup=0;//clear setup byte

	// setup device
	setup = (setup |Max11612setupByteFormatBit   |Max11612unipolar | Max11612reset_not | Max11612sel0| Max11612sel2|Max11612Clock_Int);
	config= (config | Max11612_configurationByteBit  | Max11612differential);
	send_byte2config_setup(setup, config);
} //end void init_max11612Jonas(void) {



void send_byte2config_setup(uint8_t setupByte, uint8_t configByte) {
	esp_err_t error;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (max11612_ADDRESS << 1) | I2C_MASTER_WRITE,
			1 /* expect ack */);
	i2c_master_write_byte(cmd, setupByte, 1);
	i2c_master_write_byte(cmd, configByte, 1);
	i2c_master_stop(cmd);
	error = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);


}


int readMax11612(uint8_t channel) {
	esp_err_t error;
	uint8_t msb;
	uint8_t lsb2;
	uint8_t msb2;
		uint8_t lsb;


	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (max11612_ADDRESS << 1) | I2C_MASTER_READ,1 /* expect ack */);
	i2c_master_read_byte(cmd, &msb, 0);
	i2c_master_read_byte(cmd, &lsb, 0);
	//i2c_master_read(cmd, &msb,sizeof(msb), 1);
	i2c_master_read_byte(cmd, &msb2, 0);
	i2c_master_read_byte(cmd, &lsb2, 1);
	i2c_master_stop(cmd);
	error=i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	if (error != 0)
		printf("[ADC]XXXXXXXXXXXXXXXXError in read value %d\n\n",error);


		/*printf("\n[ADC]MSB1signed decimal:%d \t, unsigned decimal %u, hex %X\n", msb, msb, msb);
		printf("\n[ADC]LSB1signed decimal:%d \t, unsigned decimal %u, hex %X\n", lsb, lsb, lsb);
		printf("\n[ADC]MSB2signed decimal:%d \t, unsigned decimal %u, hex %X\n", msb2, msb2, msb2);
				printf("\nLSB2[ADC]signed decimal:%d \t, unsigned decimal %u, hex %X\n", lsb2, lsb2, lsb2);*/


	int ret = (((msb & 0x0f) << 8) | lsb);
//	printf("[ADC]############# put together %d\n",ret);
	//printf("\n[ADC]from i2c msb:%x and lsb:%x \t", msb2,lsb2);
//	ret = (((msb2 & 0x0f) << 8) | lsb2);
//	printf("[ADC]############# 2nd put together %d\n",ret);

	return ret;

}




float x(int value) {
	float  adcMax;
	float res_div_ratio, ref_voltage, volts_out, adc_ratio;

	adcMax = 4096.0;
	ref_voltage = 2.048;
	res_div_ratio = 15.468085106;  // resistor divider 1/( 4k7/(68k+4k7))

	adc_ratio = value/ adcMax;
	volts_out= (adc_ratio*ref_voltage)*res_div_ratio;

	return volts_out;


}

void task_max11612Jonas(void *ignore) {



	while (1) {
		int temp;
		temp =readMax11612(1);

		public_tempC=(double)x(temp);
		vTaskDelay(funcTimeMiliSecs / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

