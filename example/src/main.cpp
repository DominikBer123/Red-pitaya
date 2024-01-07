#include <fstream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"
#include "version.h"


#include "test.h"
#include "lightning.h"
#include "i2c.h"

extern "C" {
    #include "rpApp.h"
}

#define CH_SIGNAL_SIZE_DEFAULT      1000

#define DISTANCE_NUM_OF_SAMPLES		1
#define LIGHT_NUM_OF_SAMPLES		  1


//Parameters
CBooleanParameter 	ss_bool_p 	(	"SS_BOOL_P", 	CBaseParameter::RW, false,	0);
CIntParameter		ss_int_p 	(	"SS_INT_P", 	CBaseParameter::RW, 100,	0,	1,65535);
CStringParameter    ss_string_p	(	"SS_STRING_P",	CBaseParameter::RW, "",		0);

//Singals
//Type     Name            Name same in js    Num of samples           Default value        
CIntSignal distance_signal("DISTANCE_SIGNAL", DISTANCE_NUM_OF_SAMPLES,    0);
CIntSignal light_signal   ("LIGHT_SIGNAL",    LIGHT_NUM_OF_SAMPLES,       0);


//Application description
const char *rp_app_desc(void)
{
	return (const char *)"Example application.\n";
}


//Application init
int rp_app_init(void)
{
	srand(time(0));
	CDataManager::GetInstance()->SetParamInterval(100);
	CDataManager::GetInstance()->SetSignalInterval(100);
	ss_bool_p.SendValue(false);
	ss_int_p.SendValue(100);
	ss_string_p.Set("TEST");

	blinkInit();

	initUart();

	initI2c();

	// GPIO init -> RELE
  if (rp_Init() != RP_OK)
  {
    fprintf(stderr, "Red Pitaya API init failed!\n");
    return EXIT_FAILURE;
  }
  rp_DpinSetDirection (RP_DIO2_P, RP_OUT);

	return 0;
}


//Application exit
int rp_app_exit(void)
{
	blinkRelease();

	return 0;
}

//Set parameters
int rp_set_params(rp_app_params_t *p, int len)
{
    return 0;
}

//Get parameters
int rp_get_params(rp_app_params_t **p)
{
    return 0;
}

//Get signals
int rp_get_signals(float ***s, int *sig_num, int *sig_len)
{
    return 0;
}

int cnt = 0;

//Update signals
void UpdateSignals(void)
{

	
	/* //Random value for testing
	int randVal = rand()%100; //0-99
	//Distance signal
	distance_signal[0] = randVal;
	randVal = cnt%8; //rant()%100; //0-99
	//Light signal
	light_signal[0] = randVal; */

	// Aquire data from sensors
	//DISTANCE SIGNAL
	uint8_t distance = readDistance();
	distance_signal[0] = distance;


	//LIGHT SIGNAL
	int light = readLux();
	light_signal[0] = light;


	// Turn on LED if distance is less than 20km
	if(distance < 20)
	{
		rp_DpinSetState(RP_DIO2_P, RP_LOW);
	}
	else
	{
		rp_DpinSetState(RP_DIO2_P, RP_HIGH);
	}


	//Blink
	blink(cnt%8, (cnt%16)/8);
	cnt++;
}


//Update parameters
void UpdateParams(void)
{
    std::fstream fs;
    fs.open ("/tmp/debug.log", std::fstream::in | std::fstream::out | std::fstream::app);

	if (ss_bool_p.IsNewValue())
	{
		ss_bool_p.Update();
		fs << "ss_bool_p = " << ss_bool_p.Value() << "\n";
	}

	if (ss_int_p.IsNewValue())
	{
		ss_int_p.Update();
        	fs << "ss_int_p = " << ss_int_p.Value() << "\n";

		ss_string_p.Set("Int value changed");
		ss_string_p.Update();
	}

	if (ss_string_p.IsNewValue())
	{
		ss_string_p.Update();
		fs << "ss_string_p = " << ss_string_p.Value() << "\n";
	}

	fs.close();

}

void PostUpdateSignals(){}

void OnNewParams(void)
{
	UpdateParams();
}

void OnNewSignals(void)
{
	UpdateSignals();
}


