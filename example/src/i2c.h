#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rp.h"

#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
extern "C" {
#endif


//Function prototype
void initI2c();
int readLux();


#ifdef __cplusplus
}
#endif

#endif // I2C_H_