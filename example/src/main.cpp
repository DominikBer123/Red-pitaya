#include <fstream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

#include "version.h"

extern "C" {
    #include "rpApp.h"
}

#define CH_SIGNAL_SIZE_DEFAULT      1000


//Parameters

CBooleanParameter 	ss_bool_p 	(	"SS_BOOL_P", 	CBaseParameter::RW, false,	0);
CIntParameter		ss_int_p 	(	"SS_INT_P", 	CBaseParameter::RW, 100,	0,	1,65535);
CStringParameter    ss_string_p	(	"SS_STRING_P",	CBaseParameter::RW, "",		0);

//Singals
CFloatSignal ss_signal_1("SS_SIGNAL_1", CH_SIGNAL_SIZE_DEFAULT, 0.0f);
CFloatSignal ss_signal_2("SS_SIGNAL_2", CH_SIGNAL_SIZE_DEFAULT, 0.0f);


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

	return 0;
}


//Application exit
int rp_app_exit(void)
{
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

//Update signals
void UpdateSignals(void)
{

	if (ss_signal_1.GetSize() != CH_SIGNAL_SIZE_DEFAULT) {
    		ss_signal_1.Resize(CH_SIGNAL_SIZE_DEFAULT);
	}

	if (ss_signal_2.GetSize() != CH_SIGNAL_SIZE_DEFAULT) {
    		ss_signal_2.Resize(CH_SIGNAL_SIZE_DEFAULT);
	}

	for(int i = 0 ; i < CH_SIGNAL_SIZE_DEFAULT ; ++i){
		float a = ((float)(rand()%10)+1.0)/10.0;
		ss_signal_1[i] = a;
    		ss_signal_2[i] = a + 1.0;
	}
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


