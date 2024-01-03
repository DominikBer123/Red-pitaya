#include <fstream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"
#include "version.h"

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>

#include "rp_hw.h"
#include "test.h"

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
CIntSignal distance_signal("DISTANCE_SIGNAL", DISTANCE_NUM_OF_SAMPLES,    7);
CIntSignal light_signal   ("LIGHT_SIGNAL",    LIGHT_NUM_OF_SAMPLES,       0);




/* Inline functions definition */
static int init_spi();
static int release_spi();
static int read_flash_id(int fd);
static int write_spi(char *write_data, int size);

/* Constants definition */
int spi_fd = -1;
char senzor_data[3];

static int init_spi(){

    /* MODES: mode |= SPI_LOOP;
    *        mode |= SPI_CPHA;
    *        mode |= SPI_CPOL;
    *                 mode |= SPI_LSB_FIRST;
    *        mode |= SPI_CS_HIGH;
    *        mode |= SPI_3WIRE;
    *        mode |= SPI_NO_CS;
    *        mode |= SPI_READY;
    *
    * multiple possibilities possible using | */
    int mode = 0;

    /* Opening file stream */
    spi_fd = open("/dev/spidev1.0", O_RDWR | O_NOCTTY);

    if(spi_fd < 0){
        printf("Error opening spidev0.1. Error: %s\n", strerror(errno));
        return -1;
    }

    /* Setting mode (CPHA, CPOL) */
    if(ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0){
        printf("Error setting SPI_IOC_RD_MODE. Error: %s\n", strerror(errno));
        return -1;
    }

    /* Setting SPI bus speed */
    int spi_speed = 250000;

    if(ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0){
        printf("Error setting SPI_IOC_WR_MAX_SPEED_HZ. Error: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

static int release_spi(){

    /* Release the spi resources */
    close(spi_fd);

    return 0;
}

/* Read data from the SPI bus */
static int read_flash_id(int fd){

    int size = 2;

    /*struct spi_ioc_transfer {
        __u64           tx_buf;
        __u64           rx_buf;

        __u32           len;
        __u32           speed_hz;

        __u16           delay_usecs;
        __u8            bits_per_word;
        __u8            cs_change;
        __u32           pad;
    }*/
    /* If the contents of 'struct spi_ioc_transfer' ever change
    * incompatibly, then the ioctl number (currently 0) must change;
    * ioctls with constant size fields get a bit more in the way of
    * error checking than ones (like this) where that field varies.
    *
    * NOTE: struct layout is the same in 64bit and 32bit userspace.*/
    struct spi_ioc_transfer xfer[size];

    unsigned char           buf0[1];
    unsigned char           buf1[3];
    int                     status;

    memset(xfer, 0, sizeof xfer);

    /* RDID command */
    buf0[0] = 0x9f;
    /* Some sample data */
    buf1[0] = 0x01;
    buf1[1] = 0x23;
    buf1[2] = 0x45;

    /* RDID buffer */
    xfer[0].tx_buf = (__u64)((__u32)buf0);
    xfer[0].rx_buf = (__u64)((__u32)buf0);
    xfer[0].len = 1;

    /* Sample loopback buffer */
    xfer[1].tx_buf = (__u64)((__u32)buf1);
    xfer[1].rx_buf = (__u64)((__u32)buf1);
    xfer[1].len = 3;

    /* ioctl function arguments
    * arg[0] - file descriptor
    * arg[1] - message number
    * arg[2] - spi_ioc_transfer structure
    */
    status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
    if (status < 0) {
        perror("SPI_IOC_MESSAGE");
        return -1;
    }

    /* Print read buffer */
    for(int i = 0; i < 3; i++){
        printf("Buffer: %d\n", buf1[i]);
		senzor_data[i] = buf1[i];
    }

    return 0;
}

/* Write data to the SPI bus */
static int write_spi(char *write_buffer, int size){

    int write_spi = write(spi_fd, write_buffer, strlen(write_buffer));

    if(write_spi < 0){
        printf("Failed to write to SPI. Error: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}



 

void* client_refresher(void* client);

void publish_callback(void** unused, struct mqtt_response_publish *published);
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
	int res = rp_I2C_InitDevice("/dev/i2c-0",0x23);
    uint8_t wb[1] = {0x01};
    res = rp_I2C_IOCTL_WriteBuffer(wb,1); // Write position for reading.
	wb[0] = 0x10;
	res = rp_I2C_IOCTL_WriteBuffer(wb,1);
	
    
    return 0;

}
//Application exit
int rp_app_exit(void)
{
	blinkRelease();
	rp_SPI_Release();
     /* Release resources */
    
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

	char senzor_cmd[1] = {0x40};
if(init_spi() < 0){
        printf("Initialization of SPI failed. Error: %s\n", strerror(errno));
        //return -1;
    }
    /* Write some sample data */
    if(write_spi(senzor_cmd, 1) < 0){
        printf("Write to SPI failed. Error: %s\n", strerror(errno));
        //return -1;
    }

    /* Read flash ID and some sample loopback data */
    if(read_flash_id(spi_fd) < 0){
        printf("Error reading from SPI bus : %s\n", strerror(errno));
        //return -1;
    }
    if(release_spi() < 0){
        printf("Relase of SPI resources failed, Error: %s\n", strerror(errno));
        //return -1;
    }
    //Distance signal
    distance_signal[0] = senzor_data[0]<<16 & senzor_data[1]<<8 & senzor_data[2];
   

	uint8_t rb[2];
    rp_I2C_IOCTL_ReadBuffer((uint8_t *)rb,2);
	int lux_value = 0;
	lux_value = rb[1]<<8 | rb[0];
    //LIGHT SIGNAL
	light_signal[0] = lux_value;
	int randVal = rand()%100; //0-99
	
	//Random value for testing
	randVal = cnt%8; //rant()%100; //0-99
	
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


