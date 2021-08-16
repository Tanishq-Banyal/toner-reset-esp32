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

EEPROM_t dev;
esp_err_t ret;
i2c_port_t i2c_port = I2C_NUM_0;

uint8_t buf1[EEPROM_SIZE];
uint8_t buf2[EEPROM_SIZE];
uint8_t buf3[EEPROM_SIZE];

// Read from a new chip and then put the bytes in writebuff array, then write back to old chip. you may modify a few numbers if u wish. current writebuff is configured for samsung scx-4200 lazer toner.
uint8_t writebuff[EEPROM_SIZE] = {67,52,50,48,48,69,88,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,57,57,57,57,57,57,0,0,67,82,85,77,45,54,54,54,54,54,54,54,54,54,54,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,67,52,50,48,48,69,88,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};


void print_data(uint8_t dt[], uint32_t len)
{
	uint32_t addr;

	for (addr = 0 ; addr < len ; addr++)
	{
		printf("%02x ", dt[addr]); fflush(stdout);
	}
/*
	uint16_t clm = 0;
	for (addr = 0; addr < len; addr++)
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

void loadROM(uint8_t* readbuff[])
{
	for (int i=0 ; i < EEPROM_SIZE ; i++)
	{
		ret = ReadRom(&dev, i, readbuff[i]);
		if (ret != ESP_OK)
		{
			ESP_LOGI(tag, "ReadRom[%d] fail %d", i, ret);
			break;
		}
	}
}

void dumpROM(uint8_t writebuff[])
{
	for (int i=0 ; i < EEPROM_SIZE ; i++)
	{
		ret = WriteRom(&dev, i, writebuff[i]);
		if (ret != ESP_OK)
		{
			ESP_LOGI(tag, "WriteRom[%d] fail %d", i, ret);
			break;
		}
	}
}

void verifyROM(uint8_t bf1[], uint8_t bf2[])
{
	uint16_t err_count = 0;
	bool verify_failed = false;

	for (int i=0 ; i < EEPROM_SIZE ; i++)
	{
		if (bf1[i] != bf2[i])
		{
			verify_failed = true; err_count += 1;
			printf("Memory Verification failed at address 0x%02x, %u != %u\n", i, bf1[i], bf2[i]);
		}
	}

	if (verify_failed)
		printf("Verification Failed with %u Errors !\n", err_count);
	else
		printf("Verified Successfully, No Errors !\n");
}

void show_menu()
{
	unsigned selection;

	puts("Available Actions :-");
	puts("");
	puts("1 --> Load EEPROM to BUFFER");
	puts("2 --> Dump BUFFER to EEPROM");
	puts("3 --> Load STDIN to BUFFER");
	puts("4 --> Dump BUFFER to STDOUT");
	puts("5 --> Compare 2 BUFFERS");
	puts("6 --> Fill BUFFER with 0s");
	puts("0 --> Reset ESP32 & BUFFERS");
	puts("");
	printf("Choose Actions : "); scanf("%u", &selection);

	switch (selection)
	{
	case 1:
		//Load EEPROM to BUFFER
		break;
	
	case 2:
		//Dump BUFFER to EEPROM
		break;
	
	case 3:
		//Load STDIN to BUFFER
		break;
	
	case 4:
		//Dump BUFFER to STDOUT
		break;
	
	case 5:
		//Compare 2 BUFFERS
		break;
	
	case 6:
		//Fill BUFFER with 0s
		break;
	
	case 0:
		//Reset ESP32 & BUFFERS
		break;
	}
}

void app_main()
{
	i2c_master_driver_initialize(&dev, EEPROM_SIZE, i2c_port,
								I2C_FREQUENCY, CONFIG_I2C_ADDRESS,
								CONFIG_SDA_GPIO, CONFIG_SCL_GPIO);
	
    //////////////////////////////////////////////

	while (true) show_menu();
}
