#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include "../lib/bme280.h"
#include <sys/ioctl.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

float temperature() {
       	int file;
        char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
    // Get I2C device, BME280 I2C address is 0x76(136)
	ioctl(file, I2C_SLAVE, 0x76);

	// Read 24 bytes of data from register(0x88)
	char reg[1] = {0x88};
	write(file, reg, 1);
	char b1[24] = {0};
	if(read(file, b1, 24) != 24)
	{
		printf("Error : Input/Output error \n");
		exit(1);
	}

	// Convert the data
	// temp coefficents
	int dig_T1 = (b1[0] + b1[1] * 256);
	int dig_T2 = (b1[2] + b1[3] * 256);
	if(dig_T2 > 32767)
	{
		dig_T2 -= 65536;
	}
	int dig_T3 = (b1[4] + b1[5] * 256);
	if(dig_T3 > 32767)
	{
		dig_T3 -= 65536;
	}

        long adc_t = ((long)(data[3] * 65536 + ((long)(data[4] * 256) + (long)(data[5] & 0xF0)))) / 16;

        // Temperature offset calculations
	float var1 = (((float)adc_t) / 16384.0 - ((float)dig_T1) / 1024.0) * ((float)dig_T2);
	float var2 = ((((float)adc_t) / 131072.0 - ((float)dig_T1) / 8192.0) *
					(((float)adc_t)/131072.0 - ((float)dig_T1)/8192.0)) * ((float)dig_T3);
	float t_fine = (long)(var1 + var2);
	float cTemp = (var1 + var2) / 5120.0;
	float fTemp = cTemp * 1.8 + 32;
        return cTemp;
}

float pressure () {
    // pressure coefficents
	int dig_P1 = (b1[6] + b1[7] * 256);
	int dig_P2 = (b1[8] + b1[9] * 256);
	if(dig_P2 > 32767)
	{
		dig_P2 -= 65536;
	}
	int dig_P3 = (b1[10] + b1[11] * 256);
	if(dig_P3 > 32767)
	{
		dig_P3 -= 65536;
	}
	int dig_P4 = (b1[12] + b1[13] * 256);
	if(dig_P4 > 32767)
	{
		dig_P4 -= 65536;
	}
	int dig_P5 = (b1[14] + b1[15] * 256);
	if(dig_P5 > 32767)
	{
	dig_P5 -= 65536;
	}
	int dig_P6 = (b1[16] + b1[17] * 256);
	if(dig_P6 > 32767)
	{
		dig_P6 -= 65536;
	}
	int dig_P7 = (b1[18] + b1[19] * 256);
	if(dig_P7 > 32767)
	{
		dig_P7 -= 65536;
	}
	int dig_P8 = (b1[20] + b1[21] * 256);
	if(dig_P8 > 32767)
	{
	dig_P8 -= 65536;
	}
	int dig_P9 = (b1[22] + b1[23] * 256);
	if(dig_P9 > 32767)
	{
		dig_P9 -= 65536;
	}

        long adc_p = ((long)(data[0] * 65536 + ((long)(data[1] * 256) + (long)(data[2] & 0xF0)))) / 16;

        // Pressure offset calculations
	var1 = ((float)t_fine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((float)dig_P6) / 32768.0;
	var2 = var2 + var1 * ((float)dig_P5) * 2.0;
	var2 = (var2 / 4.0) + (((float)dig_P4) * 65536.0);
	var1 = (((float) dig_P3) * var1 * var1 / 524288.0 + ((float) dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0) * ((float)dig_P1);
	float p = 1048576.0 - (float)adc_p;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((float) dig_P9) * p * p / 2147483648.0;
	var2 = p * ((float) dig_P8) / 32768.0;
	float pressure = (p + (var1 + var2 + ((float)dig_P7)) / 16.0) / 100;
        return pressure; 
}

float humidity() {
    // Read 1 byte of data from register(0xA1)
	reg[0] = 0xA1;
	write(file, reg, 1);
	char data[8] = {0};
	read(file, data, 1);
	int dig_H1 = data[0];

	// Read 7 bytes of data from register(0xE1)
	reg[0] = 0xE1;
	write(file, reg, 1);
	read(file, b1, 7);

	// Convert the data
	// humidity coefficents
	int dig_H2 = (b1[0] + b1[1] * 256);
	if(dig_H2 > 32767)
	{
		dig_H2 -= 65536;
	}
	int dig_H3 = b1[2] & 0xFF ;
	int dig_H4 = (b1[3] * 16 + (b1[4] & 0xF));
	if(dig_H4 > 32767)
	{
		dig_H4 -= 65536;
	}
	int dig_H5 = (b1[4] / 16) + (b1[5] * 16);
	if(dig_H5 > 32767)
	{
		dig_H5 -= 65536;
	}
	int dig_H6 = b1[6];
	if(dig_H6 > 127)
	{
		dig_H6 -= 256;
	}

	// Select control humidity register(0xF2)
	// Humidity over sampling rate = 1(0x01)
	char config[2] = {0};
	config[0] = 0xF2;
	config[1] = 0x01;
	write(file, config, 2);
	// Select control measurement register(0xF4)
	// Normal mode, temp and pressure over sampling rate = 1(0x27)
	config[0] = 0xF4;
	config[1] = 0x27;
	write(file, config, 2);
	// Select config register(0xF5)
	// Stand_by time = 1000 ms(0xA0)
	config[0] = 0xF5;
	config[1] = 0xA0;
	write(file, config, 2);

	long adc_h = (data[6] * 256 + data[7]);

        // Humidity offset calculations
	float var_H = (((float)t_fine) - 76800.0);
	var_H = (adc_h - (dig_H4 * 64.0 + dig_H5 / 16384.0 * var_H)) * (dig_H2 / 65536.0 * (1.0 + dig_H6 / 67108864.0 * var_H * (1.0 + dig_H3 / 67108864.0 * var_H)));
	float humidity = var_H * (1.0 -  dig_H1 * var_H / 524288.0);
	if(humidity > 100.0)
	{
		humidity = 100.0;
	}else
	if(humidity < 0.0) 
	{
		humidity = 0.0;
	}
        return humidity;
}


int main() {

    /* create array of our sensor addresses
    unsigned char sensor_addrs[] = {BME280_ADDR, 
                                    ICM20948_ADDR, 
                                    LTR390_ADDR,
                                    SGP40_ADDR, 
                                    TSL2591_ADDR};
    */

    unsigned char sensor_addrs[] = {0x76, 0x68, 0x29, 0x53, 0x43};

    // traverse array of sensors addresses in hex fmt
    for (uint8_t i = 0; i < sizeof(sensor_addrs); i++) {
        // check if addrs in array are found
        //int sensor_init = wiringPiI2CSetup(sensor_addrs[i]);
        printf("RET: %d \n", sensor_init);

        if (sensor_init < 0) {
            printf("[!] I2C Device not found\n");
            return -1; 
        }

        else {
            printf("[+] I2C device found @ addr : %X \n", sensor_addrs[i]);
        }
    }

    // Create I2C bus
    int file;
    char *bus = "/dev/i2c-1";
    if((file = open(bus, O_RDWR)) < 0) 
    {
		printf("Failed to open the bus. \n");
		exit(1);
    }

    // Read 8 bytes of data from register(0xF7)
    // pressure msb1, pressure msb, pressure lsb, temp msb1, temp msb, temp lsb, humidity lsb, humidity msb
    reg[0] = 0xF7;
    write(file, reg, 1);
    read(file, data, 8);

    printf("\nrun i2cdetect -y 1 and notice 0x43 does not appear\n");
    printf("see: https://github.com/WiringPi/WiringPi/blob/master/wiringPi/wiringPiI2C.c#L220\n");
    printf("looks like `wiringPiI2CSetup()` does not handle non-existent addresses...\n");

    return 0;
}

