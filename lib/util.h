#ifndef __UTIL_H__
#define __UTIL_H__
#include <stddef.h>

// possible struct to hold address/dev info?

/**
 * @brief Function to scan for available I2C address on given bus 
 * @param bus File path to i2c-x device under /dev
 * @param found_addrs Array of found addresses
 * @param num_found_addrs Holds number of found available addresses
 */
void i2c_scan(const char *bus, unsigned char found_addrs[], size_t *num_found_addrs);

/**
 * @brief Function to initialize a specified I2C device as a bus
 * @param bus File path to i2c-x device under /dev
 * @param addresses Array of I2C addresses on our bus in hex fmt
 * @param num_addrs Size of the addresses array
 * @return The /dev device as a "file"
 */
int i2c_init(const char *bus, unsigned char addresses[], size_t num_addrs);

#endif
