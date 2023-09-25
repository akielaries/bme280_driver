/**
 * main driver for interacting with waveshare board
 */

#include <stdio.h>
#include "lib/util.h"

int main() {
    // I2C device
    const char* i2c_bus = "/dev/i2c-1";
    // I2C device addresses
    unsigned char sensor_addrs[] = {0x76, 0x68, 0x29, 0x53, 0x28};

    // pass in the I2C device, array of addresses, and the number of addresses
    if (i2c_init(i2c_bus, sensor_addrs, sizeof(sensor_addrs))) {
        printf("Initialization complete.\n");
    } else {
        printf("Initialization failed.\n");
    }

    return 0;
}

