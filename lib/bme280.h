#ifndef __BME280_H__
#define __BME280_H__

#define BME280_ADDR             0x76

typedef struct Sensors {
    // data array
    char data[8];
    // b1 register?
    char b1[24];

} Sensors;


float temperature();

float pressure();

float humidity();

float altitude();


#endif
