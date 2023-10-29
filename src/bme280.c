#include "../lib/bme280.h"
#include "../lib/addrs.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

void bme280(const HWInfo *hw_info) {
    // printf("BMEM280 ADDR: 0x%02X \n", hw_info->i2c_addrs[0]);

    // I2C device bus file, READ/WRITE
    int file = open(hw_info->i2c_bus, O_RDWR);

    // failed to open
    if (file < 0) {
        perror("[!] - Failed to open I2C device file...");
        exit(EXIT_FAILURE);
    }

    // file open
    else {
        // allocate memory for BMEData struct holding variables used in all BME
        // functions
        BMEData *bme_data = (BMEData *)malloc(sizeof(BMEData));

        // set file descriptor
        bme_data->file = file;

        // get I2C device, BME280 I2C address saved in hw_info->i2c_addrs
        // should return 0 on success -1 on failure?
        ioctl(file, I2C_SLAVE, hw_info->i2c_addrs[0]);

        // block data
        // allocate memory for data blocks of 24 bytes
        // char block_data[24] = {0};
        bme_data->block_data = (char *)malloc(24 * sizeof(char));

        // sensor data
        // allocate memory for data of 8 bytes
        // char data[8] = {0};
        bme_data->data = (char *)malloc(8 * sizeof(char));

        // make calls to the actual sensor read functions...
        float temp = temperature(bme_data);

        // get current time... UNIX timestamp? readable time?
        time_t time_now = time(0);

        printf("time: %ld temp = %f \n", time_now, temp);

        // each of the variables above will contribute to 1 row in a CSV file
        // time         temp    press   hum     alt
        // 1698535389   33.24   1032.6  29.95   6,909

        // free memory allocated to bme_data->block_data and data
        free(bme_data->block_data);
        free(bme_data->data);
        // free memory allocated to bme_data pointer
        free(bme_data);
    }
}

float temperature(BMEData *bme_data) {
    // declare value for register(0x88)
    char temp_register[1] = {0x88};

    // write 1 byte to file from temperature register
    write(bme_data->file, temp_register, 1);

    // read 24 bytes from file
    int file_read = read(bme_data->file, bme_data->block_data, 24);

    // failed to read 24 bytes
    if (file_read != 24) {
        printf("[!] - Input/Output error \n");
        exit(EXIT_FAILURE);
    }

    else {
        // Convert the data
        // temp coefficents
        int dig_T1 = (bme_data->block_data[0] + bme_data->block_data[1] * 256);
        int dig_T2 = (bme_data->block_data[2] + bme_data->block_data[3] * 256);

        if (dig_T2 > 32767) {
            dig_T2 -= 65536;
        }

        int dig_T3 = (bme_data->block_data[4] + bme_data->block_data[5] * 256);
        if (dig_T3 > 32767) {
            dig_T3 -= 65536;
        }

        // TODO I am not 100% sure on what all of this means but it might be
        // resuable for the other readings? config array for oversampling
        char temp_config[2] = {0};
        temp_config[0] = 0xF4;
        temp_config[1] = 0x27;
        write(bme_data->file, temp_config, 2);
        // select config register(0xF5)
        temp_config[0] = 0xF5;
        // stand_by time = 1000 ms(0xA0)
        temp_config[1] = 0xA0;

        write(bme_data->file, temp_config, 2);

        // Read 8 bytes of data from register(0xF7)
        // pressure msb1, pressure msb, pressure lsb, temp msb1, temp msb, temp
        // lsb, humidity lsb, humidity msb
        temp_register[0] = 0xF7;
        // write 1 byte
        write(bme_data->file, temp_register, 1);
        // read 8 bytes
        read(bme_data->file, bme_data->data, 8);

        long adc_t = ((long)(bme_data->data[3] * 65536 +
                             ((long)(bme_data->data[4] * 256) +
                              (long)(bme_data->data[5] & 0xF0)))) /
                     16;

        // Temperature offset calculations
        float var1 = (((float)adc_t) / 16384.0 - ((float)dig_T1) / 1024.0) *
                     ((float)dig_T2);
        float var2 = ((((float)adc_t) / 131072.0 - ((float)dig_T1) / 8192.0) *
                      (((float)adc_t) / 131072.0 - ((float)dig_T1) / 8192.0)) *
                     ((float)dig_T3);
        float t_fine = (long)(var1 + var2);
        float temp_c = (var1 + var2) / 5120.0;
        float temp_f = temp_c * 1.8 + 32;

        // return temp in fahrenheit
        return temp_f;
    }
}

/*
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

        long adc_p = ((long)(data[0] * 65536 + ((long)(data[1] * 256) +
(long)(data[2] & 0xF0)))) / 16;

        // Pressure offset calculations
        var1 = ((float)t_fine / 2.0) - 64000.0;
        var2 = var1 * var1 * ((float)dig_P6) / 32768.0;
        var2 = var2 + var1 * ((float)dig_P5) * 2.0;
        var2 = (var2 / 4.0) + (((float)dig_P4) * 65536.0);
        var1 = (((float) dig_P3) * var1 * var1 / 524288.0 + ((float) dig_P2) *
var1) / 524288.0; var1 = (1.0 + var1 / 32768.0) * ((float)dig_P1); float p =
1048576.0 - (float)adc_p; p = (p - (var2 / 4096.0)) * 6250.0 / var1; var1 =
((float) dig_P9) * p * p / 2147483648.0; var2 = p * ((float) dig_P8) / 32768.0;
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
        var_H = (adc_h - (dig_H4 * 64.0 + dig_H5 / 16384.0 * var_H)) * (dig_H2 /
65536.0 * (1.0 + dig_H6 / 67108864.0 * var_H * (1.0 + dig_H3 / 67108864.0 *
var_H))); float humidity = var_H * (1.0 -  dig_H1 * var_H / 524288.0);
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
    // pressure msb1, pressure msb, pressure lsb, temp msb1, temp msb, temp lsb,
humidity lsb, humidity msb reg[0] = 0xF7; write(file, reg, 1); read(file, data,
8);

    printf("\nrun i2cdetect -y 1 and notice 0x43 does not appear\n");
    printf("see:
https://github.com/WiringPi/WiringPi/blob/master/wiringPi/wiringPiI2C.c#L220\n");
    printf("looks like `wiringPiI2CSetup()` does not handle non-existent
addresses...\n");

    return 0;
}
*/
