/*
 * spi.h
 *
 *  Created on: 10 Jun 2013
 *      Author: Remi KEAT
 */

/**
 * \file    spi.h
 * \brief   Header file for the SPI functions
 * \author  Remi KEAT
 */

#ifndef SPI_H_
#define SPI_H_

//#include "ftd2xx.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <unistd.h>
#include "spi_commands.h"
#include "globals.h"
#include "hdt.h"

SPIHandle SPIInit(int deviceNumber);
int SPIRelease(SPIHandle spiHandle);
int SPISend(SPIHandle spiHandle, size_t length, const uint8_t data[]);
int SPISendDefault(size_t length, const uint8_t data[]);
int SPIRead(SPIHandle spiHandle, size_t length, uint8_t* data[]);
int SPIReadDefault(size_t length, uint8_t* data[]);

#endif /* SPI_H_ */
