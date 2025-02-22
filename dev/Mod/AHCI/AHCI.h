/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	File: AHCI.h
	Purpose: AHCI protocol defines.

	Revision History:

	03/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <Mod/ACPI/ACPI.h>

/// @file AHCI.h
/// @brief AHCI support.

#define kAHCISectorSize (512)

struct HbaPort;
struct FisData;
struct FisRegD2H;
struct FisRegH2D;

/// @brief Frame information type.
enum
{
	kFISTypeRegH2D	 = 0x27, // Register FIS - host to device
	kFISTypeRegD2H	 = 0x34, // Register FIS - device to host
	kFISTypeDMAAct	 = 0x39, // DMA activate FIS - device to host
	kFISTypeDMASetup = 0x41, // DMA setup FIS - bidirectional
	kFISTypeData	 = 0x46, // Data FIS - bidirectional
	kFISTypeBIST	 = 0x58, // BIST activate FIS - bidirectional
	kFISTypePIOSetup = 0x5F, // PIO setup FIS - device to host
	kFISTypeDevBits	 = 0xA1, // Set device bits FIS - device to host
};

enum
{
	kAHCICmdIdentify   = 0xEC,
	kAHCICmdReadDma	   = 0xC8,
	kAHCICmdReadDmaEx  = 0x25,
	kAHCICmdWriteDma   = 0xCA,
	kAHCICmdWriteDmaEx = 0x35
};

typedef struct FisRegH2D final
{
	// DWORD 0
	NeOS::UInt8 FisType; // FIS_TYPE_REG_H2D

	NeOS::UInt8 PortMul : 4;   // Port multiplier
	NeOS::UInt8 Reserved0 : 3; // Reserved
	NeOS::UInt8 CmdOrCtrl : 1; // 1: Command, 0: Control

	NeOS::UInt8 Command;	// Command register
	NeOS::UInt8 FeatureLow; // Feature register, 7:0

	// DWORD 1
	NeOS::UInt8 Lba0;	// LBA low register, 7:0
	NeOS::UInt8 Lba1;	// LBA mid register, 15:8
	NeOS::UInt8 Lba2;	// LBA high register, 23:16
	NeOS::UInt8 Device; // Device register

	// DWORD 2
	NeOS::UInt8 Lba3;		 // LBA register, 31:24
	NeOS::UInt8 Lba4;		 // LBA register, 39:32
	NeOS::UInt8 Lba5;		 // LBA register, 47:40
	NeOS::UInt8 FeatureHigh; // Feature register, 15:8

	// DWORD 3
	NeOS::UInt8 CountLow;  // Count register, 7:0
	NeOS::UInt8 CountHigh; // Count register, 15:8
	NeOS::UInt8 Icc;	   // Isochronous command completion
	NeOS::UInt8 Control;   // Control register

	// DWORD 4
	NeOS::UInt8 Reserved1[4]; // Reserved
} FisRegH2D;

typedef struct FisRegD2H final
{
	// DWORD 0
	NeOS::UInt8 FisType; // FIS_TYPE_REG_D2H

	NeOS::UInt8 PortMul : 4;   // Port multiplier
	NeOS::UInt8 Reserved0 : 2; // Reserved
	NeOS::UInt8 IE : 1;		   // Interrupt bit
	NeOS::UInt8 Reserved1 : 1; // Reserved

	NeOS::UInt8 Status; // Status register
	NeOS::UInt8 Error;	// Error register

	// DWORD 1
	NeOS::UInt8 Lba0;	// LBA low register, 7:0
	NeOS::UInt8 Lba1;	// LBA mid register, 15:8
	NeOS::UInt8 Lba2;	// LBA high register, 23:16
	NeOS::UInt8 Device; // Device register

	// DWORD 2
	NeOS::UInt8 Lba3; // LBA register, 31:24
	NeOS::UInt8 Lba4; // LBA register, 39:32
	NeOS::UInt8 Lba5; // LBA register, 47:40
	NeOS::UInt8 Rsv2; // Reserved

	// DWORD 3
	NeOS::UInt8 CountLow;  // Count register, 7:0
	NeOS::UInt8 CountHigh; // Count register, 15:8
	NeOS::UInt8 Rsv3[2];   // Reserved

	// DWORD 4
	NeOS::UInt8 Rsv4[4]; // Reserved
} FisRegD2H;

typedef struct FisData final
{
	// DWORD 0
	NeOS::UInt8 FisType; // FIS_TYPE_DATA

	NeOS::UInt8 PortMul : 4;   // Port multiplier
	NeOS::UInt8 Reserved0 : 4; // Reserved

	NeOS::UInt8 Reserved1[2]; // Reserved

	// DWORD 1 ~ N
	NeOS::UInt32 Data[1]; // Payload
} FisData;

typedef struct FisPioSetup final
{
	// DWORD 0
	NeOS::UInt8 FisType; // FIS_TYPE_PIO_SETUP

	NeOS::UInt8 PortMul : 4;   // Port multiplier
	NeOS::UInt8 Reserved0 : 1; // Reserved
	NeOS::UInt8 DTD : 1;	   // Data transfer direction, 1 - device to host
	NeOS::UInt8 IE : 1;		   // Interrupt bit
	NeOS::UInt8 Reserved1 : 1;

	NeOS::UInt8 Status; // Status register
	NeOS::UInt8 Error;	// Error register

	// DWORD 1
	NeOS::UInt8 Lba0;	// LBA low register, 7:0
	NeOS::UInt8 Lba1;	// LBA mid register, 15:8
	NeOS::UInt8 Lba2;	// LBA high register, 23:16
	NeOS::UInt8 Device; // Device register

	// DWORD 2
	NeOS::UInt8 Lba3; // LBA register, 31:24
	NeOS::UInt8 Lba4; // LBA register, 39:32
	NeOS::UInt8 Lba5; // LBA register, 47:40
	NeOS::UInt8 Rsv2; // Reserved

	// DWORD 3
	NeOS::UInt8 CountLow;  // Count register, 7:0
	NeOS::UInt8 CountHigh; // Count register, 15:8
	NeOS::UInt8 Rsv3;	   // Reserved
	NeOS::UInt8 EStatus;   // New value of status register

	// DWORD 4
	NeOS::UInt16 TranferCount; // Transfer count
	NeOS::UInt8	 Rsv4[2];	   // Reserved
} FisPioSetup;

typedef struct FisDmaSetup final
{
	// DWORD 0
	NeOS::UInt8 FisType; // FIS_TYPE_DMA_SETUP

	NeOS::UInt8 PortMul : 4;	// Port multiplier
	NeOS::UInt8 Reserved0 : 1;	// Reserved
	NeOS::UInt8 DTD : 1;		// Data transfer direction, 1 - device to host
	NeOS::UInt8 IE : 1;			// Interrupt bit
	NeOS::UInt8 AutoEnable : 1; // Auto-activate. Specifies if DMA Activate FIS is needed

	NeOS::UInt8 Reserved1[2]; // Reserved

	// DWORD 1&2
	volatile NeOS::UInt64 DmaBufferId; // DMA Buffer Identifier. Used to Identify DMA buffer in
									   // host memory. SATA Spec says host specific and not in
									   // Spec. Trying AHCI spec might work.

	// DWORD 3
	NeOS::UInt32 Rsvd; // More reserved

	// DWORD 4
	NeOS::UInt32 DmabufOffset; // Byte offset into buffer. First 2 bits must be 0

	// DWORD 5
	NeOS::UInt32 TransferCount; // Number of bytes to transfer. Bit 0 must be 0

	// DWORD 6
	NeOS::UInt32 Reserved3; // Reserved
} FisDmaSetup;

typedef struct FisDevBits final
{
	// DWORD 0
	NeOS::UInt8 FisType; // FIS_TYPE_DMA_SETUP (A1h)

	NeOS::UInt8 Reserved0 : 5; // Reserved
	NeOS::UInt8 R0 : 1;
	NeOS::UInt8 IE : 1;
	NeOS::UInt8 N : 1;

	NeOS::UInt8 StatusLow : 3;
	NeOS::UInt8 R1 : 1;
	NeOS::UInt8 StatusHigh : 3;

	NeOS::UInt8 R2 : 1;
	NeOS::UInt8 Error;

	// DWORD 1
	NeOS::UInt32 Act;
} FisDevBits;

/// \brief Enable AHCI device bit in GHC register.
#ifndef kSATAGHC_AE
#define kSATAGHC_AE (31)
#endif //! ifndef kSATAGHC_AE

typedef struct HbaPort final
{
	NeOS::UInt32 Clb;			// 0x00, command list base address, 1K-byte aligned
	NeOS::UInt32 Clbu;			// 0x04, command list base address upper 32 bits
	NeOS::UInt32 Fb;			// 0x08, FIS base address, 256-byte aligned
	NeOS::UInt32 Fbu;			// 0x0C, FIS base address upper 32 bits
	NeOS::UInt32 Is;			// 0x10, interrupt status
	NeOS::UInt32 Ie;			// 0x14, interrupt enable
	NeOS::UInt32 Cmd;			// 0x18, command and status
	NeOS::UInt32 Reserved0;		// 0x1C, Reserved
	NeOS::UInt32 Tfd;			// 0x20, task file data
	NeOS::UInt32 Sig;			// 0x24, signature
	NeOS::UInt32 Ssts;			// 0x28, SATA status (SCR0:SStatus)
	NeOS::UInt32 Sctl;			// 0x2C, SATA control (SCR2:SControl)
	NeOS::UInt32 Serr;			// 0x30, SATA error (SCR1:SError)
	NeOS::UInt32 Sact;			// 0x34, SATA active (SCR3:SActive)
	NeOS::UInt32 Ci;			// 0x38, command issue
	NeOS::UInt32 Sntf;			// 0x3C, SATA notification (SCR4:SNotification)
	NeOS::UInt32 Fbs;			// 0x40, FIS-based switch control
	NeOS::UInt32 Reserved1[11]; // 0x44 ~ 0x6F, Reserved
	NeOS::UInt32 Vendor[4];		// 0x70 ~ 0x7F, vendor specific
} HbaPort;

typedef struct HbaMem final
{
	// 0x00 - 0x2B, Generic Host Control
	NeOS::UInt32 Cap;	  // 0x00, Host capability
	NeOS::UInt32 Ghc;	  // 0x04, Global host control
	NeOS::UInt32 Is;	  // 0x08, Interrupt status
	NeOS::UInt32 Pi;	  // 0x0C, Port implemented
	NeOS::UInt32 Vs;	  // 0x10, Version
	NeOS::UInt32 Ccc_ctl; // 0x14, Command completion coalescing control
	NeOS::UInt32 Ccc_pts; // 0x18, Command completion coalescing ports
	NeOS::UInt32 Em_loc;  // 0x1C, Enclosure management location
	NeOS::UInt32 Em_ctl;  // 0x20, Enclosure management control
	NeOS::UInt32 Cap2;	  // 0x24, Host capabilities extended
	NeOS::UInt32 Bohc;	  // 0x28, BIOS/OS handoff control and status

	NeOS::UInt8 Resv0[0xA0 - 0x2C];
	NeOS::UInt8 Vendor[0x100 - 0xA0];

	HbaPort Ports[32]; // 1 ~ 32, 32 is the max ahci devices per controller.
} HbaMem;

typedef struct HbaCmdHeader final
{
	// DW0
	NeOS::UInt8 Cfl : 5;		  // Command FIS length in DWORDS, 2 ~ 16
	NeOS::UInt8 Atapi : 1;		  // ATAPI
	NeOS::UInt8 Write : 1;		  // Write, 1: H2D, 0: D2H
	NeOS::UInt8 Prefetchable : 1; // Prefetchable

	NeOS::UInt8 Reset : 1;	   // Reset
	NeOS::UInt8 BIST : 1;	   // BIST
	NeOS::UInt8 Clear : 1;	   // Clear busy upon R_OK
	NeOS::UInt8 Reserved0 : 1; // Reserved
	NeOS::UInt8 Pmp : 4;	   // Port multiplier port

	NeOS::UInt16		  Prdtl; // Physical region descriptor table length in entries
	volatile NeOS::UInt32 Prdbc; // Physical region descriptor byte count transferred

	NeOS::UInt32 Ctba;	// Command table descriptor base address
	NeOS::UInt32 Ctbau; // Command table descriptor base address upper 32 bits
} HbaCmdHeader;

typedef struct HbaFis final
{
	// 0x00
	FisDmaSetup Dsfis; // DMA Setup FIS
	NeOS::UInt8 Pad0[4];
	// 0x20
	FisPioSetup Psfis; // PIO Setup FIS
	NeOS::UInt8 Pad1[12];
	// 0x40
	FisRegD2H	Rfis; // Register – Device to Host FIS
	NeOS::UInt8 Pad2[4];
	// 0x58
	FisDevBits Sdbfis; // Set Device Bit FIS
	// 0x60
	NeOS::UInt8 Ufis[64];
	// 0xA0
	NeOS::UInt8 Rsv[0x100 - 0xA0];
} HbaFis;

typedef struct HbaPrdtEntry final
{
	NeOS::UInt32 Dba;		// Data base address
	NeOS::UInt32 Dbau;		// Data base address upper 32 bits
	NeOS::UInt32 Reserved0; // Reserved
	// DW3
	NeOS::UInt32 Dbc : 22;		// Byte count, 4M max
	NeOS::UInt32 Reserved1 : 9; // Reserved
	NeOS::UInt32 Ie : 1;		// Interrupt on completion
} HbaPrdtEntry;

typedef struct HbaCmdTbl final
{
	NeOS::UInt8			Cfis[64]; // Command FIS
	NeOS::UInt8			Acmd[16]; // ATAPI command, 12 or 16 bytes
	NeOS::UInt8			Rsv[48];  // Reserved
	struct HbaPrdtEntry Prdt[];	  // Physical region descriptor table entries, 0 ~ 65535
} HbaCmdTbl;

/// @brief Initializes an AHCI disk.
/// @param PortsImplemented the amount of port that have been detected.
/// @return
NeOS::Boolean drv_std_init(NeOS::UInt16& PortsImplemented);

NeOS::Boolean drv_std_detected(NeOS::Void);

/// @brief Read from AHCI disk.
/// @param lba
/// @param buf
/// @param sector_sz
/// @param buf_sz
/// @return
NeOS::Void drv_std_read(NeOS::UInt64 lba, NeOS::Char* buf, NeOS::SizeT sector_sz, NeOS::SizeT buf_sz);

/// @brief Write to AHCI disk.
/// @param lba
/// @param buf
/// @param sector_sz
/// @param buf_sz
/// @return
NeOS::Void drv_std_write(NeOS::UInt64 lba, NeOS::Char* buf, NeOS::SizeT sector_sz, NeOS::SizeT buf_sz);

/// @brief Gets the sector count from AHCI disk.
NeOS::SizeT drv_get_sector_count();

/// @brief Gets the AHCI disk size.
NeOS::SizeT drv_get_size();

/// @brief Checks if the drive has completed the command.
BOOL drv_is_ready(void);

/* EOF */
