#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_log.h"

#include "at24c.h"

#define	EEPROM_SIZE			512		//512 bytes
#define CONFIG_SDA_GPIO		21
#define CONFIG_SCL_GPIO		22
#define CONFIG_I2C_ADDRESS	0x50
#define I2C_FREQUENCY		100000	//100 khz

void dump(uint8_t* dt, uint32_t n)
{
	uint32_t addr;

	for (addr = 0 ; addr < n ; addr++)
	{
		printf("%02x ", dt[addr]); fflush(stdout);
	}
/*
	uint16_t clm = 0;
	for (addr = 0; addr < n; addr++)
	{
		if (clm == 0) printf("0x%03x: ", addr);

		printf("0x%02x ", dt[addr]);
		clm++;
		if (clm == 16)
		{
			clm = 0;
			printf("\n");
		}
	}
*/
}

void app_main()
{
	ESP_LOGI(tag, "EEPROM is 24C%.02d",EEPROM_SIZE);
	ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(tag, "CONFIG_I2C_ADDRESS=0x%x",CONFIG_I2C_ADDRESS);

	EEPROM_t dev;
	i2c_port_t i2c_port = I2C_NUM_0;
	i2c_master_driver_initialize(&dev, EEPROM_SIZE, i2c_port,
								I2C_FREQUENCY, CONFIG_I2C_ADDRESS,
								CONFIG_SDA_GPIO, CONFIG_SCL_GPIO);
	
	esp_err_t ret;
	uint8_t readbuff[EEPROM_SIZE];
	uint8_t writebuff[EEPROM_SIZE] = {67,52,50,48,48,69,88,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,57,57,57,57,57,57,0,0,67,82,85,77,45,54,54,54,54,54,54,54,54,54,54,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,67,52,50,48,48,69,88,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
	
    //////////////////////////////////////////////

	// write rom
	puts("writing...");
	for (int i=0 ; i < EEPROM_SIZE ; i++)
	{
		ret = WriteRom(&dev, i, writebuff[i]);
		if (ret != ESP_OK)
		{
			ESP_LOGI(tag, "WriteRom[%d] fail %d", i, ret);
			while(1) { vTaskDelay(1); }
		}
	}

    /////////////////////////////////////////////

	// read rom
	puts("readback...");
	for (int i=0 ; i < EEPROM_SIZE ; i++)
	{
		ret = ReadRom(&dev, i, &readbuff[i]);
		if (ret != ESP_OK)
		{
			ESP_LOGI(tag, "ReadRom[%d] fail %d", i, ret);
			while(1) { vTaskDelay(1); }
		}
	}
	//dump(readbuff, EEPROM_SIZE);

	puts("verifying...");
	for (int i=0 ; i < EEPROM_SIZE ; i++)
	{
		if (readbuff[i] != writebuff[i])
		{
			printf("Memory Verification failed at address 0x%02x, %u != %u\n", i, readbuff[i], writebuff[i]);
		}
	}
	printf("Memory Verified !\n");
}
