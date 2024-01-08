#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rp.h"

#ifndef LIGHTNING_H_
#define LIGHTNING_H_

#ifdef __cplusplus
extern "C" {
#endif


//Function prototype
void initLightning();
uint8_t readDistance();


#ifdef __cplusplus
}
#endif

#endif // LIGHTNING_H_