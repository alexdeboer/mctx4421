/*
 * servos_and_motors.c
 *
 *  Created on: 10 Jun 2013
 *      Author: Remi KEAT
 */

/**
 * \file    servos_and_motors.c
 * \brief   Defines functions to control servos and motors
 * \author  Remi KEAT
 */

#include "servos_and_motors.h"

/**
 * \brief   Initialize given servo
 * \param   DeviceSemantics semantics
 * \return  SERVOHandle servoHandle
 */
SERVOHandle SERVOInit(DeviceSemantics semantics)
{
//	SERVOHandle servoHandle;
	int servoNum;
	//servo_type* servo = HDT_FindEntry(semantics, &servoNum);
	HDTDevSERVO_t* pservo;
	pservo = (HDTDevSERVO_t*)HDTLoadSERVO(HDT_FILE, semantics);
	uint8_t data[] = {SPIServoInitCmd, (pservo->regaddr)<<4, (pservo->freq)>>8, (pservo->freq)&0xFF};
	SPISendDefault(sizeof(data), data);
//	servoHandle.servoNum = servoNum;
	return (SERVOHandle)pservo;
}


/**
 * \brief   Release given servos
 * \param   SERVOHandle handle
 * \return  int retVal : always 0
 */
int SERVORelease (SERVOHandle handle)
{
	HDTDevSERVO_t* pservo = (HDTDevSERVO_t*)handle;
	uint8_t data[] = {SPIServoReleaseCmd, (pservo->regaddr)<<4, 0x00, 0x00};
	SPISendDefault(sizeof(data), data);
	HDTClearSERVO(pservo);
	return 0;
}

/**
 * \brief   Set the given servos to the same given angle
 * \param   SERVOHandle handle
 * \param   int angle : valid values = 0-360
 * \return  int retVal : always 0
 */
int SERVOSet (SERVOHandle handle, int angle)
{
	HDTDevSERVO_t* pservo = (HDTDevSERVO_t*)handle;
	uint8_t data[] = {SPIServoSetCmd, (pservo->regaddr)<<4, angle>>8, angle&0xFF};
	fprintf(stderr,"New\n");
	//uint8_t data[] = {SPIServoSetCmd, (pservo->regaddr)<<4, angle>>8, angle};
	SPISendDefault(sizeof(data), data);
	return 0;
}

/**
 * \brief   Initialize given motor
 * \param   DeviceSemantics semantics
 * \return  MOTORHandle motorHandle
 */
MOTORHandle MOTORInit(DeviceSemantics semantics)
{
//	MOTORHandle motorHandle;
	int motorNum, motorFrequency;
	//motor_type* motor = HDT_FindEntry(semantics, &motorNum);
	HDTDevMOTOR_t* pmotor;
	pmotor = (HDTDevMOTOR_t*)HDTLoadMOTOR(HDT_FILE, semantics);
	if (pmotor)
	{
		pmotor->ptable = (HDTTable_t*)HDTLoadTable(HDT_FILE, (HDTDevice_t*)pmotor);
		if (pmotor->ptable)
		{
			motorNum = pmotor->regaddr;
			motorFrequency = pmotor->freq;
			uint8_t data[] = {SPIMotorInitCmd, motorNum<<4, motorFrequency>>8, motorFrequency&0xFF};
			SPISendDefault(sizeof(data), data);
		}
		else
		{
			free(pmotor);
			pmotor = NULL;
		}
	}
	return (MOTORHandle)pmotor;
}

/**
 * \brief   Release given motor
 * \param   MOTORHandle handle
 * \return  int retVal : always 0
 */
int MOTORRelease(MOTORHandle handle)
{
	HDTDevMOTOR_t* pmotor = (HDTDevMOTOR_t*)handle;
	uint8_t data[] = {SPIMotorReleaseCmd, (pmotor->regaddr)<<4, 0x00, 0x00};
	SPISendDefault(sizeof(data), data);
	HDTClearMOTOR(pmotor);
	return 0;
}

/**
 * \brief   Set the given motors to the same given speed
 * \param   MOTORHandle handle
 * \param   int speed : motor speed in percent
 * \details Valid values for speed : 
 * - -100 to 100 (full backward to full forward)
 * - 0 for full stop
 * 
 * \return  int retVal : always 0
 */
int MOTORDrive(MOTORHandle handle, int speed)
{
	HDTDevMOTOR_t* pmotor = (HDTDevMOTOR_t*)handle;
	int direction;
	if (speed >= 0)
		direction = 0;
	else
		direction = 1;
	uint8_t data[] = {SPIMotorSetCmd, ((pmotor->regaddr)<<4)+(direction&0x0F), abs(speed)>>8, abs(speed)&0xFF};
	SPISendDefault(sizeof(data), data);
	return 0;
}

/**
 * \brief  Initialize given Quadrature-Decoder (up to 8 decoders are possible)
 * \param  DeviceSemantics semantics
 * \return QUADHandle handle : QUADHandle or 0 for error
 */
QUADHandle QUADInit(DeviceSemantics semantics)
{
//	QUADHandle handle;
	//int quadNum;
	HDTDevENCODER_t* penc;
	penc = (HDTDevENCODER_t*)HDTLoadENCODER(HDT_FILE, semantics);
	if (penc)
	{
		penc->pmotor = (HDTDevMOTOR_t*)MOTORInit(penc->motorname);
	}
	//quad_type* quad = HDT_FindEntry(semantics, &quadNum);
//	handle.quadNum = quadNum;
	return (QUADHandle)penc;
}

/**
 * \brief   Read actual Quadrature-Decoder counter, initially zero.
 * \param   QUADHandle handle : ONE decoder-handle
 * \return  long value of the encoder
 */
long QUADRead(QUADHandle handle)
{
	HDTDevENCODER_t* penc = (HDTDevENCODER_t*)handle;
	uint8_t data[] = {SPIReadEncoderCmd, (penc->regaddr)<<4, 0x00, 0x00};
	uint8_t* reading[4];
	long result = 0;
	SPISendDefault(sizeof(data), data);
	SPIReadDefault(4, reading);
	result = ((*reading)[0])<<24;
	result += ((*reading)[1])<<16;
	result += ((*reading)[2])<<8;
	result += ((*reading)[3]);
	return result;
}

/**
 * \brief		Reset one or more Quadrature-Decoder
 * \param   QUADHandle handle : logical-or of decoder-handles to be reseted
 * \return  int retVal : 0 = ok\n
            -1 = error wrong handle
 */
int QUADReset(QUADHandle handle)
{
	HDTDevENCODER_t* penc = (HDTDevENCODER_t*)handle;
	uint8_t data[] = {SPIResetEncoderCmd, (penc->regaddr)<<4, 0x00, 0x00};
	SPISendDefault(sizeof(data), data);
	return 0;
}

/**
 * \brief    Release one or more Quadrature-Decoder
 * \param    QUADHandle handle : logical-or of decoder-handles to be released
 * \return   int retVal : 0 = ok\n
             -1 = error wrong handle
 */
int QUADRelease(QUADHandle handle)
{
	HDTClearENCODER((HDTDevENCODER_t*)handle);
	return 0;
}

/**
 * \brief			Get the semantic of the corresponding motor
 * \param     QUADHandle handle : ONE decoder-handle
 * \details   0 = wrong handle
 * \return    DeviceSemantics semantic : Of the corresponding motor
 */
DeviceSemantics QUADGetMotor(QUADHandle handle)
{
	return ((HDTDevENCODER_t*)handle)->motorname;
}
