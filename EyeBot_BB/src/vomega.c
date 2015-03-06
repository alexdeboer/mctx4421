/*
 * vomega.c
 *
 *  Created on: 3 Sep 2013
 *      Author: "Remi KEAT"
 */

/**
 * \file		vomega.c
 * \brief		Defines the VW functions
 * \author  Remi KEAT
 */

#include "vomega.h"

/**
 * \brief		Initialize given VW-Driver (only 1 can be initialized!).
 * 					The motors and encoders are automatically reserved!!
 * 					The Timescale allows to adjust the tradeoff between accuracy (scale=1, update at 100Hz) and speed(scale>1, update at 100/scale Hz).
 * \param   DeviceSemantics semantics
 * \param   int Timescale : prescale value for 100Hz IRQ (1 to ...)
 * \return  VWHandle handle : VWHandle or 0 for error
 */
VWHandle VWInit(DeviceSemantics semantics, int Timescale)
{
	//TODO: Complete
	long encoderParam;
	HDTDevDRIVE_t* pdrive;
	pdrive = (HDTDevDRIVE_t*)HDTLoadDRIVE(HDT_FILE, semantics);
	pdrive->penc1 = (HDTDevENCODER_t*)QUADInit(pdrive->encname1);
	pdrive->penc2 = (HDTDevENCODER_t*)QUADInit(pdrive->encname2);
	encoderParam = (pdrive->penc1->regaddr)+(pdrive->penc2->regaddr)<<4;
	if (pdrive->encname3 != "")
	{
		pdrive->penc3 = (HDTDevENCODER_t*)QUADInit(pdrive->encname3);
		if (pdrive->penc3)
		{
			encoderParam = encoderParam + (pdrive->penc3->regaddr)<<8;
		}
	}
	if (pdrive->encname4 != "")
	{
		pdrive->penc4 = (HDTDevENCODER_t*)QUADInit(pdrive->encname4);
		if (pdrive->penc4)
		{
			encoderParam = encoderParam + (pdrive->penc3->regaddr)<<12;
		}
	}
	uint8_t data0[] = {SPIVWInitCmd, ((pdrive->drivetype)<<4)+(SPIWheelDist1Param&0x0F), (pdrive->wheeldist1)>>8, (pdrive->wheeldist1)&0xFF};
	SPISendDefault(sizeof(data0), data0);
	uint8_t data1[] = {SPIVWInitCmd, ((pdrive->drivetype)<<4)+(SPIWheelDist2Param&0x0F), (pdrive->wheeldist2)>>8, (pdrive->wheeldist2)&0xFF};
	SPISendDefault(sizeof(data1), data1);
	uint8_t data2[] = {SPIVWInitCmd, ((pdrive->drivetype)<<4)+(SPIAxesDistParam&0x0F), (pdrive->axesdist)>>8, (pdrive->axesdist)&0xFF};
	SPISendDefault(sizeof(data2), data2);
	uint8_t data3[] = {SPIVWInitCmd, ((pdrive->drivetype)<<4)+(SPIEncoderParam&0x0F), encoderParam>>8, encoderParam&0xFF};
	SPISendDefault(sizeof(data3), data3);
	return (VWHandle)pdrive;
}

/**
 * \brief		Drives distance "delta" with speed v straight ahead (forward or backward)
 * 					any subsequent call of VWDriveStraight, -Turn, -Curve or VWSetSpeed while this one is still being executed,
 * 					results in an immediate interruption of this command
 * \param   VWHandle handle : ONE VWHandle
 * \param   meter delta : distance to drive in m
 * \param   meterPerSec v : speed to drive with (always positive!)
 * \details delta :
 * 					- pos. -> forward
 * 					- neg. -> backward
 *
 * \return	int retVal :\n
 * 					0 = ok\n
 * 					-1 = error wrong handle
 */
int VWDriveStraight(VWHandle handle, meter delta, meterPerSec v)
{
	HDTDevDRIVE_t* pdrive = (HDTDevDRIVE_t*)handle;
	uint8_t data0[] = {SPIVWDriveStraightCmd, SPIDistanceParam, ((int)(delta*1000))>>8, ((int)(delta*1000))&0xFF};
	SPISendDefault(sizeof(data0), data0);
	uint8_t data1[] = {SPIVWDriveStraightCmd, SPISpeedParam, ((int)(v*1000))>>8, ((int)(v*1000))&0xFF};
	SPISendDefault(sizeof(data1), data1);
	return 0;
}

/**
 * \brief		Turns about "delta" with speed w on the spot (clockwise or counter-clockwise)
 * 					any subsequent call of VWDriveStraight, -Turn, -Curve or VWSetSpeed while this one is still being executed,
 * 					results in an immediate interruption of this command
 * \param   VWHandle handle : ONE VWHandle
 * \param   radians delta : degree to turn in radians
 *
 *
 * \param   radPerSec w : speed to turn with (always positive!)
 * \details delta :
 * 					- pos. -> counter-clockwise
 * 					- neg. -> clockwise
 *
 * \return  int retVal :\n
 * 					0 = ok\n
            -1 = error wrong handle
 */
int VWDriveTurn(VWHandle handle, radians delta, radPerSec w)
{
	//TODO: Complete
	return 0;
}

/**
 * \brief		Blocks the calling process until the previous VWDriveX() command has been completed
 * \param   VWHandle handle : ONE VWHandle
 * \return  int retVal :\n
 * -1 = error wrong handle\n
 * 0 = previous VWDriveX command has been completed
 */
int VWDriveWait(VWHandle handle)
{
	uint8_t data[] = {SPIVWDriveWaitCmd, 0x00, 0x00, 0x00};
	SPISendDefault(sizeof(data), data);
	sleep(3);
	return 0;
}
