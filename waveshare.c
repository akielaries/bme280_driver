/**
 * main driver for interacting with waveshare board
 */

#include "lib/addrs.h"
#include "lib/bme280.h"
#include "lib/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // I2C device
    const char *i2c_bus = "/dev/i2c-1";
    // I2C device addresses from addrs.h
    unsigned char sensor_addrs[] = {BME280_ADDR,
                                    ICM20948_ADDR,
                                    LTR390_ADDR,
                                    SGP40_ADDR,
                                    TSL2591_ADDR};

    // MAIN LOOP
    while (1) {
        // automatic memory allocation
        // create struct object for Hardware info
        // HWInfo hw_info;

        // dynamic memory allocation
        // allocate memory for the Hardware Info struct
        HWInfo *hw_info = (HWInfo *)malloc(sizeof(HWInfo));

        if (hw_info == NULL) {
            perror("[!] - Memory allocation failed for HWInfo pointer...\n");
            exit(EXIT_FAILURE);
        }

        // hw_info pointer is populated
        hw_info->i2c_bus = i2c_bus;
        hw_info->i2c_addrs = sensor_addrs;
        hw_info->num_addrs = sizeof(sensor_addrs);

        // pass in the I2C device, array of addresses, and the number of
        // addresses
        i2c_init(hw_info->i2c_bus, hw_info->i2c_addrs, hw_info->num_addrs);

        // call main bme280 function with I2C address
        bme280(hw_info);

        /* calls to the other sensors functions would look something like
        this: icm20948(hw_info); ltr390(hw_info); sgp40(hw_info);
        tsl2591(hw_info);
        */

        // sleep for 5 seconds
        sleep(5);

        // free hw_info memory
        // TODO this memory will never be freed unless the binary is stopped while running
        free(hw_info);
    }
    return 0;
}
