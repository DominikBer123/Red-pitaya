#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include "rp.h"
#include "rp_hw.h"
#include "i2c.h"

void initI2c()
{
  int res = rp_I2C_InitDevice("/dev/i2c-0",0x23);
  uint8_t wb[1] = {0x01};
  res = rp_I2C_IOCTL_WriteBuffer(wb,1); // Write position for reading.
  // Mode 0001_0000
	wb[0] = 0x10;
	res = rp_I2C_IOCTL_WriteBuffer(wb,1);
}

int readLux()
{
  uint8_t rb[2];
    rp_I2C_IOCTL_ReadBuffer((uint8_t *)rb,2);
	int lux_value = 0;
  // for mode 0001_0000 divide with 1.2
	lux_value = ((rb[0]<<8) | rb[1]) * 10 / 12;
  
  return lux_value;
}