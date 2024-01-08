#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "lightning.h"

#define SPI_DEVICE "/dev/spidev1.0"

void setSPIMode(int spi_fd, uint8_t spi_mode);
void transferSPI(int spi_fd, uint8_t *tx_buffer, uint8_t *rx_buffer, size_t len);


int spi_fd;

// Example: Sending and receiving data over SPI
uint8_t tx_data[] = {0x47, 0x00}; // Example data to send
uint8_t rx_data[sizeof(tx_data)];

void initLightning()
{
  // Open SPI device
  if ((spi_fd = open(SPI_DEVICE, O_RDWR)) < 0) {
    perror("Error opening SPI device");
    return EXIT_FAILURE;
  }

  // Set SPI mode
  uint8_t spi_mode = SPI_MODE_1; // Set your desired SPI mode
  setSPIMode(spi_fd, spi_mode);
}

uint8_t readDistance()
{
  transferSPI(spi_fd, tx_data, rx_data, sizeof(tx_data));

  // Print rx_data
  for (int i = 0; i < sizeof(rx_data); i++) {
      printf("[%d]: %d ", i, rx_data[i]);
  }
  printf("\n");

  usleep(1000);

  uint8_t distance = rx_data[1];

  distance &= 0x3F;

  return distance;
}


void setSPIMode(int spi_fd, uint8_t spi_mode) {
  if (ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode) == -1) {
    perror("Error setting SPI mode");
    exit(EXIT_FAILURE);
  }
}

void transferSPI(int spi_fd, uint8_t *tx_buffer, uint8_t *rx_buffer, size_t len) {
    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)tx_buffer,
        .rx_buf = (unsigned long)rx_buffer,
        .len = len,
        .speed_hz = 200000, // Set your desired SPI clock speed
        .delay_usecs = 0,
        .bits_per_word = 8,
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        perror("SPI transfer failed");
        exit(EXIT_FAILURE);
    }
}
