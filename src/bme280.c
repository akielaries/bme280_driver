#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <wiringPiI2C.h>
#include "../lib/bme280.h"

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
        int sensor_init = wiringPiI2CSetup(sensor_addrs[i]);
        printf("RET: %d \n", sensor_init);

        if (sensor_init < 0) {
            printf("[!] I2C Device not found\n");
            return -1; 
        }

        else {
            printf("[+] I2C device found @ addr : %X \n", sensor_addrs[i]);
        }

    }
    
    printf("\nrun i2cdetect -y 1 and notice 0x43 does not appear\n");
    printf("see: https://github.com/WiringPi/WiringPi/blob/master/wiringPi/wiringPiI2C.c#L220\n");
    printf("looks like `wiringPiI2CSetup()` does not handle non-existent addresses...\n");

    return 0;
}

