#ifndef __BME280_H__
#define __BME280_H__
#include "addrs.h"

typedef struct BMEData {
    // file descriptor
    int file;
    // data array
    char *data;
    // b1, block data
    char *block_data;
} BMEData;

void bme280(const HWInfo *hw_info);

float temperature(BMEData *bme_data);

float pressure();

float humidity();

float altitude();

#endif
