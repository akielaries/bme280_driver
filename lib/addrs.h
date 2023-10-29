#ifndef ADDRS_H
#define ADDRS_H
#include <stddef.h>

/**
 * WaveShare board I2C sensor addresses
 */
#define BME280_ADDR 0x76
#define ICM20948_ADDR 0x68
#define LTR390_ADDR 0x53
#define SGP40_ADDR 0x28
#define TSL2591_ADDR 0x29

typedef struct HWInfo {
    const char *i2c_bus;
    unsigned char *i2c_addrs;
    size_t num_addrs;
} HWInfo;

#endif
