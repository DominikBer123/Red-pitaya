#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rp.h"
#include "test.h"

int unsigned led = 0;

int blinkInit()
{
  // Initialization of API
  if (rp_Init() != RP_OK) 
  {
    fprintf(stderr, "Red Pitaya API init failed!\n");
    return EXIT_FAILURE;
  }
}

int blink(uint8_t ledNum, uint8_t ledState)
{
  led = ledNum;
  
  fprintf(stderr, "Set led[%u] to [%u]\n", ledNum, ledState);

  if(ledState == 0) rp_DpinSetState(ledNum, RP_LOW);
  else if(ledState == 1) rp_DpinSetState(ledNum, RP_HIGH);

  return 1;
}

int blinkRelease()
{
  // Releasing resources
  rp_Release();
}

