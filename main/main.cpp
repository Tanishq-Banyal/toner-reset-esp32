#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <sstream>
#include <iostream>

using namespace std;

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_log.h"

#include "24c0x.h"

#define	EEPROM_SIZE			512		//512 bytes
#define CONFIG_SDA_GPIO		21
#define CONFIG_SCL_GPIO		22
#define CONFIG_I2C_ADDRESS	0x50
#define I2C_FREQUENCY		100000	//100 khz

EEPROM_t dev;
esp_err_t ret;
i2c_port_t i2c_port = I2C_NUM_0;

uint8_t buf1[EEPROM_SIZE] = {};
uint8_t buf2[EEPROM_SIZE] = {};
uint8_t buf3[EEPROM_SIZE] = {67,52,50,48,48,69,88,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,57,57,57,57,57,57,0,0,67,82,85,77,45,54,54,54,54,54,54,54,54,54,54,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,67,52,50,48,48,69,88,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
// Read from a new chip and then put the bytes in writebuff array, then write back to old chip. you may modify a few numbers if u wish. current writebuff is configured for samsung scx-4200 lazer toner.

vector<string> split_string(string &str, const char delim)
{
	string tok;
	vector<string> tokens;
	
	for (auto &ch : str)
	{
		if (ch == delim)
		{
			tokens.push_back(tok);
			tok.clear();
		}
		else
		{
			tok += ch;
		}
	}
	
	tokens.push_back(tok);
	return tokens;
}

void scan_data(uint8_t dt[], uint16_t len)
{
	char delim;
	string str;
	cout << "Enter Deliminator : ";
	cin >> delim; flush(cin);
	cout << "Enter Data : "; getline(cin, str);
	
	vector<uint8_t> bytes;
	vector<string> tokens = split_string(str, delim);
	
	for (auto &tok : tokens)
	{
		try
		{
			bytes.push_back(stoul(tok));
		}
		catch (exception &e)
		{
			cout << e.what() << ", assuming 0" << endl;
			bytes.push_back(0);
		}
	}
	
	for (int i=0 ; i < len ; i++)
	{
		dt[i] = bytes[i];
	}
}

void print_data(uint8_t dt[], uint16_t len)
{
	//uint16_t clm = 0;
	for (uint16_t i=0 ; i < len ; i++)
	{
		printf("%02x ", dt[i]); fflush(stdout);
		/*
		if (clm == 0) printf("0x%03x: ", addr);

		printf("0x%02x ", dt[addr]);
		clm++;
		if (clm == 16)
		{
			clm = 0;
			printf("\n");
		}
		*/
	}
}

void loadROM(uint8_t buf[], uint16_t len)
{
	for (int i=0 ; i < len ; i++)
	{
		ret = ReadRom(&dev, i, &buf[i]);
		if (ret != ESP_OK)
		{
			ESP_LOGI(tag, "ReadRom[%d] fail %d", i, ret);
			break;
		}
	}
}

void dumpROM(uint8_t buf[], uint16_t len)
{
	for (int i=0 ; i < len ; i++)
	{
		ret = WriteRom(&dev, i, buf[i]);
		if (ret != ESP_OK)
		{
			ESP_LOGI(tag, "WriteRom[%d] fail %d", i, ret);
			break;
		}
	}
}

void clearROM(uint8_t buf[], uint16_t start, uint16_t end)
{
	for (int i=start ; i < end ; i++)
	{
		buf[i] = 0;
	}
}

void verifyROM(uint8_t bf1[], uint8_t bf2[], uint16_t len)
{
	uint16_t err_count = 0;
	bool verify_failed = false;

	for (int i=0 ; i < len ; i++)
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

uint8_t* buffer_select_menu(uint8_t bn)
{
	unsigned selection;

	printf("Select Buffer %u :-", bn);
	puts("");
	puts("BUFFER 1");
	puts("BUFFER 2");
	puts("BUFFER 3");
	puts("");
	puts("Enter Selection : "); scanf("%u", &selection);

	switch (selection)
	{
	case 1:
		return buf1;
	
	case 2:
		return buf2;
	
	case 3:
		return buf3;
	
	default:
		puts("Invalid Input, Please Try Again !");
		return buffer_select_menu(bn);
	}
}

void main_menu()
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
		loadROM(buffer_select_menu(1), EEPROM_SIZE);
		break;
	
	case 2:
		//Dump BUFFER to EEPROM
		dumpROM(buffer_select_menu(1), EEPROM_SIZE);
		break;
	
	case 3:
		//Load STDIN to BUFFER
		scan_data(buffer_select_menu(1), EEPROM_SIZE);
		break;
	
	case 4:
		//Dump BUFFER to STDOUT
		print_data(buffer_select_menu(1), EEPROM_SIZE);
		break;
	
	case 5:
		//Compare 2 BUFFERS
		verifyROM(buffer_select_menu(1), buffer_select_menu(2), EEPROM_SIZE);
		break;
	
	case 6:
		//Fill BUFFER with 0s
		clearROM(buffer_select_menu(1), 0, EEPROM_SIZE);
		break;
	
	case 0:
		//Reset ESP32 & BUFFERS
		esp_restart(); break;
	
	default:
		puts("Invalid Input, Please Try Again !");
	}
}

void app_main()
{
	i2c_master_driver_initialize(&dev, EEPROM_SIZE, i2c_port,
								I2C_FREQUENCY, CONFIG_I2C_ADDRESS,
								CONFIG_SDA_GPIO, CONFIG_SCL_GPIO);
	
    //////////////////////////////////////////////

	while (true) main_menu();
}
