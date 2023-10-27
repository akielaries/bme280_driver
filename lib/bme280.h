#ifndef __BME280_H__
#define __BME280_H__

#define BME280_ADDR             0x76


float temperature();

float pressure();

float humidity();

float altitude();


#endif
