#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rp.h"
#include "rp_hw.h"
#include "lightning.h"


char rx_buf[2] = {0};
int size = 2;

void initUart()
{
  int res = rp_UartInit(); // init uart api
  printf("Init result: %d\n",res);

  res = rp_UartSetTimeout(10); // set timeout in 1/10 sec. 10 = 1 sec
  printf("Set timeout: %d\n",res);

  res = rp_UartSetSpeed(9600); // set uart speed
  printf("Set speed: %d\n",res);

  res = rp_UartSetBits(RP_UART_CS8); // set word size
  printf("Set CS8: %d\n",res);

  res = rp_UartSetStopBits(RP_UART_STOP1); // set stop bits
  printf("Set Stop Bits 2: %d\n",res);

  res = rp_UartSetParityMode(RP_UART_NONE); // set parity
  printf("Set Parity Mode: %d\n",res);

  res = rp_UartSetSettings(); // apply settings to uart
  printf("Set settings: %d\n",res);

  // GPIO init -> REQUEST LINE
  if (rp_Init() != RP_OK)
  {
    fprintf(stderr, "Red Pitaya API init failed!\n");
    return EXIT_FAILURE;
  }
  // configure DIO[0:7]_N to inputs
  rp_DpinSetDirection (RP_DIO1_P, RP_OUT);
    
}

uint8_t readDistance()
{
  //Request
  rp_DpinSetState(RP_DIO1_P, RP_HIGH);
  usleep(1000);
  rp_DpinSetState(RP_DIO1_P, RP_LOW);

  //Read
  int res = rp_UartRead((unsigned char*)rx_buf,&size); // read from uart
  //printf("Read result: %d\n",res);
  printf("Size: %d\n",size);

  uint8_t distance = rx_buf[0];
  printf("Distance: %d\n", distance);

  return distance;
}
