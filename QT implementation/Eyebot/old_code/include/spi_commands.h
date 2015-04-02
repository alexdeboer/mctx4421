/*
 * spi_commands.h
 *
 *  Created on: 11 Jun 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file    spi_commands.h
 * \brief   Defines the OP-codes for the SPI messages
 * \author  Remi KEAT
 */

#ifndef SPI_COMMANDS_H_
#define SPI_COMMANDS_H_

#define SPIServoInitCmd 0x01
#define SPIServoReleaseCmd 0x02
#define SPIServoSetCmd 0x03
#define SPIMotorInitCmd 0x04
#define SPIMotorReleaseCmd 0x05
#define SPIMotorSetCmd 0x06
#define SPIReadEncoderCmd 0x07
#define SPIPSDGetCmd 0x08
#define SPILatchSetupCmd 0x09
#define SPILatchBankSetupCmd 0x0A
#define SPILatchReadCmd 0x0B
#define SPILatchWriteCmd 0x0C
#define SPIResetEncoderCmd 0x0D
#define SPIVWInitCmd 0x0E
#define SPIVWDriveStraightCmd 0x0F
#define SPIVWDriveWaitCmd 0x10

#define SPIWheelDist1Param 0x01
#define SPIWheelDist2Param 0x02
#define SPIAxesDistParam 0x03
#define SPIEncoderParam 0x04

#define SPIDistanceParam 0x05
#define SPISpeedParam 0x06

#endif /* SPI_COMMANDS_H_ */
