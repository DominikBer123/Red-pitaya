#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rp.h"

#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

int blinkInit();
int blink(uint8_t ledNum, uint8_t ledState);
int blinkRelease();



#ifdef __cplusplus
}
#endif

#endif // TEST_H