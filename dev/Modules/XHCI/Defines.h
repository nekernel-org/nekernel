/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

	File: Defines.h
	Purpose: XHCI (and backwards) header.

	Revision History:

	01/02/24: Added file (amlel)
	03/02/24: Update filename to Defines.h (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

using namespace Kernel;

#define kUSBCommand				 (UInt16)0x0
#define kUSBStatus				 (UInt16)0x2
#define kUSBInterruptEnable		 (UInt16)0x4
#define kUSBFrameNum			 (UInt16)0x6
#define kUSBFrameListBaseAddress (UInt16)0x8
#define kUSBFrameModifyStart	 (UInt16)0xC
#define kUSBPort1StatusCtrl		 (UInt16)0x10
#define kUSBPort2StatusCtrl		 (UInt16)0x12

typedef struct USBCommandRegister final
{
	UInt8 mReserved[8]; // Reserved
	UInt8
		  mMaxPacket; //  	0 = Max packet size 32 bits 1 = Max packet size 64 bits
	UInt8 mConfigure;
	UInt8 mSoftwareDebug;
	UInt8 mGlobalResume;
	UInt8 mGlobalSuspend;
	UInt8 mHostCtrlReset;
	UInt8 mRun; //  	1 = Controller execute frame list entries
} USBCommandRegister;

typedef struct USBStatusRegister final
{
	UInt8 mReserved[8]; // Reserved
	UInt8 mHalted;		//  1 = bit 0 in CMD is zero 0 = bit 0 in CMD is 1
	UInt8 mProcessError;
	UInt8 mSystemError;
	UInt8 mResumeDetected;
	UInt8 mErrorInterrupt;
	UInt8 mInterrupt;
} USBStatusRegister;

typedef struct USBInterruptEnableRegister final
{
	UInt8 mReserved[4]; // Reserved
	UInt8 mShortPacket; //  1=Enable interrupt 0=Disable interrupt
	UInt8 mComplete;	//  1=Enable interrupt 0=Disable interrupt
	UInt8 mResume;		//  1=Enable interrupt 0=Disable interrupt
	UInt8 mTimeoutCRC;	//  1=Enable interrupt 0=Disable interrupt
} USBInterruptEnableRegister;

/*
	Some terminology:

	Frame Number: Number of processed entry of the Frame List.
	Frame List Base Address:
	32-bit physical adress of Frame List. Remember that first 12 bytes are
   always 0. The Frame List must contain 1024 entries.
*/
