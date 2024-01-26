/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <CompilerKit/Compiler.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

#include <KernelKit/Device.hpp>

#define kDriveInvalidID -1
#define kDriveNameLen 32

namespace hCore
{
    enum
    {
        kInvalidDrive = -1,
        kBlockDevice = 0xAD,
        kMassStorage = 0xDA,
        kFloppyDisc = 0xCD,
        kOpticalDisc = 0xDC, // CD-ROM/DVD-ROM/Blu-Ray
        kReadOnly = 0x10, // Read only drive
        kXPMDrive = 0x11, // eXplicit Partition Map.
        kXPTDrive = 0x12, // GPT w/ XPM partition.
    	kMBRDrive = 0x13, // IBM PC classic partition scheme
    };

    typedef Int64 DriveID;

    // Mounted drive.
    struct DriveTraits final
    {
        char fName[kDriveNameLen]; // /system, /boot...
        Int32 fKind; // fMassStorage, fFloppy, fOpticalDisc.
        DriveID fId; // Drive id.
        Int32 fFlags; // fReadOnly, fXPMDrive, fXPTDrive

		//! disk mount, unmount operations
		void(*fMount)(void);
		void(*fUnmount)(void);

        bool(*fReady)(void); //! is drive ready?
    	
		//! for StorageKit.
		struct 
		{
			voidPtr fPacketContent; // packet body.
			Char fPacketMime[32]; //! identify what we're sending.
			SizeT fPacketSize; // packet size
		} fPacket;
    };

#define kPacketBinary "file/x-binary"
#define kPacketSource "file/x-source"
#define kPacketASCII  "file/x-ascii"
#define kPacketZip    "file/x-zip"
    
    //! drive as a device.
    typedef DeviceInterface<DriveTraits> Drive;
    typedef Drive* DrivePtr;

    class DriveSelector final
    {
    public:
    	explicit DriveSelector();
    	~DriveSelector();
    	
    public:
    	HCORE_COPY_DEFAULT(DriveSelector);

    	DriveTraits& GetMounted();
    	bool Mount(DriveTraits* drive);
		DriveTraits* Unmount();

    private:
    	DriveTraits* fDrive;
    	
    };
}
