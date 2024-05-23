/* -------------------------------------------

	Copyright SoftwareLabs

	File: Defines.hxx
	Purpose: AHCI header.

	Revision History:

	03/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <Builtins/ACPI/ACPI.hxx>

// Forward declarations of structs.

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
	NewOS::UInt8 FisType; // FIS_TYPE_REG_H2D

	NewOS::UInt8 PortMul : 4;	// Port multiplier
	NewOS::UInt8 Reserved0 : 3; // Reserved
	NewOS::UInt8 CmdOrCtrl : 1; // 1: Command, 0: Control

	NewOS::UInt8 Command;  // Command register
	NewOS::UInt8 Featurel; // Feature register, 7:0

	// DWORD 1
	NewOS::UInt8 Lba0;	 // LBA low register, 7:0
	NewOS::UInt8 Lba1;	 // LBA mid register, 15:8
	NewOS::UInt8 Lba2;	 // LBA high register, 23:16
	NewOS::UInt8 Device; // Device register

	// DWORD 2
	NewOS::UInt8 Lba3;		  // LBA register, 31:24
	NewOS::UInt8 Lba4;		  // LBA register, 39:32
	NewOS::UInt8 Lba5;		  // LBA register, 47:40
	NewOS::UInt8 FeatureHigh; // Feature register, 15:8

	// DWORD 3
	NewOS::UInt8 CountLow;	// Count register, 7:0
	NewOS::UInt8 CountHigh; // Count register, 15:8
	NewOS::UInt8 Icc;		// Isochronous command completion
	NewOS::UInt8 Control;	// Control register

	// DWORD 4
	NewOS::UInt8 Reserved1[4]; // Reserved
} FisRegH2D;

typedef struct FisRegD2H final
{
	// DWORD 0
	NewOS::UInt8 FisType; // FIS_TYPE_REG_D2H

	NewOS::UInt8 PortMul : 4;	   // Port multiplier
	NewOS::UInt8 Reserved0 : 2;	   // Reserved
	NewOS::UInt8 InterruptBit : 1; // Interrupt bit
	NewOS::UInt8 Reserved1 : 1;	   // Reserved

	NewOS::UInt8 Status; // Status register
	NewOS::UInt8 Rrror;	 // Error register

	// DWORD 1
	NewOS::UInt8 Lba0;	 // LBA low register, 7:0
	NewOS::UInt8 Lba1;	 // LBA mid register, 15:8
	NewOS::UInt8 Lba2;	 // LBA high register, 23:16
	NewOS::UInt8 Device; // Device register

	// DWORD 2
	NewOS::UInt8 Lba3; // LBA register, 31:24
	NewOS::UInt8 Lba4; // LBA register, 39:32
	NewOS::UInt8 Lba5; // LBA register, 47:40
	NewOS::UInt8 Rsv2; // Reserved

	// DWORD 3
	NewOS::UInt8 CountLow;	// Count register, 7:0
	NewOS::UInt8 CountHigh; // Count register, 15:8
	NewOS::UInt8 Rsv3[2];	// Reserved

	// DWORD 4
	NewOS::UInt8 Rsv4[4]; // Reserved
} FisRegD2H;

typedef struct FisData final
{
	// DWORD 0
	NewOS::UInt8 FisType; // FIS_TYPE_DATA

	NewOS::UInt8 PortMul : 4;	// Port multiplier
	NewOS::UInt8 Reserved0 : 4; // Reserved

	NewOS::UInt8 Reserved1[2]; // Reserved

	// DWORD 1 ~ N
	NewOS::UInt32 Data[1]; // Payload
} FisData;

typedef struct FisPioSetup final
{
	// DWORD 0
	NewOS::UInt8 FisType; // FIS_TYPE_PIO_SETUP

	NewOS::UInt8 PortMul : 4;	   // Port multiplier
	NewOS::UInt8 Reserved0 : 1;	   // Reserved
	NewOS::UInt8 DTD : 1;		   // Data transfer direction, 1 - device to host
	NewOS::UInt8 InterruptBit : 1; // Interrupt bit
	NewOS::UInt8 Reserved1 : 1;

	NewOS::UInt8 Status; // Status register
	NewOS::UInt8 Error;	 // Error register

	// DWORD 1
	NewOS::UInt8 Lba0;	 // LBA low register, 7:0
	NewOS::UInt8 Lba1;	 // LBA mid register, 15:8
	NewOS::UInt8 Lba2;	 // LBA high register, 23:16
	NewOS::UInt8 Device; // Device register

	// DWORD 2
	NewOS::UInt8 Lba3; // LBA register, 31:24
	NewOS::UInt8 Lba4; // LBA register, 39:32
	NewOS::UInt8 Lba5; // LBA register, 47:40
	NewOS::UInt8 Rsv2; // Reserved

	// DWORD 3
	NewOS::UInt8 CountLow;	// Count register, 7:0
	NewOS::UInt8 CountHigh; // Count register, 15:8
	NewOS::UInt8 Rsv3;		// Reserved
	NewOS::UInt8 EStatus;	// New value of status register

	// DWORD 4
	NewOS::UInt16 TranferCount; // Transfer count
	NewOS::UInt8  Rsv4[2];		// Reserved
} FisPioSetup;

typedef struct FisDmaSetup final
{
	// DWORD 0
	NewOS::UInt8 FisType; // FIS_TYPE_DMA_SETUP

	NewOS::UInt8 PortMul : 4;	   // Port multiplier
	NewOS::UInt8 Reserved0 : 1;	   // Reserved
	NewOS::UInt8 DTD : 1;		   // Data transfer direction, 1 - device to host
	NewOS::UInt8 InterruptBit : 1; // Interrupt bit
	NewOS::UInt8 AutoEnable : 1;   // Auto-activate. Specifies if DMA Activate FIS is needed

	NewOS::UInt8 Reserved1[2]; // Reserved

	// DWORD 1&2
	NewOS::UInt64 DmaBufferId; // DMA Buffer Identifier. Used to Identify DMA buffer in
							   // host memory. SATA Spec says host specific and not in
							   // Spec. Trying AHCI spec might work.

	// DWORD 3
	NewOS::UInt32 Rsvd; // More reserved

	// DWORD 4
	NewOS::UInt32 DmabufOffset; // Byte offset into buffer. First 2 bits must be 0

	// DWORD 5
	NewOS::UInt32 TransferCount; // Number of bytes to transfer. Bit 0 must be 0

	// DWORD 6
	NewOS::UInt32 Reserved3; // Reserved
} FisDmaSetup;

typedef struct FisDevBits final
{
	// DWORD 0
	NewOS::UInt8 FisType; // FIS_TYPE_DMA_SETUP (A1h)

	NewOS::UInt8 Reserved0 : 5; // Reserved
	NewOS::UInt8 R0 : 1;
	NewOS::UInt8 InterruptBit : 1;
	NewOS::UInt8 N : 1;

	NewOS::UInt8 StatusLow : 3;
	NewOS::UInt8 R1 : 1;
	NewOS::UInt8 StatusHigh : 3;

	NewOS::UInt8 R2 : 1;
	NewOS::UInt8 Error;

	// DWORD 1
	NewOS::UInt32 Act;
} FisDevBits;

/// \brief Enable AHCI device bit in GHC register.
#ifndef kAhciGHC_AE
#define kAhciGHC_AE (31)
#endif //! ifndef kAhciGHC_AE

typedef struct HbaPort final
{
	NewOS::UInt32 Clb;			 // 0x00, command list base address, 1K-byte aligned
	NewOS::UInt32 Clbu;			 // 0x04, command list base address upper 32 bits
	NewOS::UInt32 Fb;			 // 0x08, FIS base address, 256-byte aligned
	NewOS::UInt32 Fbu;			 // 0x0C, FIS base address upper 32 bits
	NewOS::UInt32 Is;			 // 0x10, interrupt status
	NewOS::UInt32 Ie;			 // 0x14, interrupt enable
	NewOS::UInt32 Cmd;			 // 0x18, command and status
	NewOS::UInt32 Reserved0;	 // 0x1C, Reserved
	NewOS::UInt32 Tfd;			 // 0x20, task file data
	NewOS::UInt32 Sig;			 // 0x24, signature
	NewOS::UInt32 Ssts;			 // 0x28, SATA status (SCR0:SStatus)
	NewOS::UInt32 Sctl;			 // 0x2C, SATA control (SCR2:SControl)
	NewOS::UInt32 Serr;			 // 0x30, SATA error (SCR1:SError)
	NewOS::UInt32 Sact;			 // 0x34, SATA active (SCR3:SActive)
	NewOS::UInt32 Ci;			 // 0x38, command issue
	NewOS::UInt32 Sntf;			 // 0x20, SATA notification (SCR4:SNotification)
	NewOS::UInt32 Fbs;			 // 0x40, FIS-based switch control
	NewOS::UInt32 Reserved1[11]; // 0x44 ~ 0x6F, Reserved
	NewOS::UInt32 Vendor[4];	 // 0x70 ~ 0x7F, vendor specific
} HbaPort;

typedef struct HbaMem final
{
	// 0x00 - 0x2B, Generic Host Control
	NewOS::UInt32 Cap;	   // 0x00, Host capability
	NewOS::UInt32 Ghc;	   // 0x04, Global host control
	NewOS::UInt32 Is;	   // 0x08, Interrupt status
	NewOS::UInt32 Pi;	   // 0x0C, Port implemented
	NewOS::UInt32 Vs;	   // 0x10, Version
	NewOS::UInt32 Ccc_ctl; // 0x14, Command completion coalescing control
	NewOS::UInt32 Ccc_pts; // 0x18, Command completion coalescing ports
	NewOS::UInt32 Em_loc;  // 0x1C, Enclosure management location
	NewOS::UInt32 Em_ctl;  // 0x20, Enclosure management control
	NewOS::UInt32 Cap2;	   // 0x24, Host capabilities extended
	NewOS::UInt32 Bohc;	   // 0x28, BIOS/OS handoff control and status

	NewOS::UInt16 Resv0;
	NewOS::UInt32 Resv2;

	HbaPort Ports[1]; // 1 ~ 32
} HbaMem;

typedef struct HbaCmdHeader final
{
	// DW0
	NewOS::UInt8 Cfl : 5;		   // Command FIS length in DWORDS, 2 ~ 16
	NewOS::UInt8 Atapi : 1;		   // ATAPI
	NewOS::UInt8 Write : 1;		   // Write, 1: H2D, 0: D2H
	NewOS::UInt8 Prefetchable : 1; // Prefetchable

	NewOS::UInt8 Reset : 1;		// Reset
	NewOS::UInt8 BIST : 1;		// BIST
	NewOS::UInt8 Clear : 1;		// Clear busy upon R_OK
	NewOS::UInt8 Reserved0 : 1; // Reserved
	NewOS::UInt8 Pmp : 4;		// Port multiplier port

	NewOS::UInt16		   Prdtl; // Physical region descriptor table length in entries
	volatile NewOS::UInt32 Prdbc; // Physical region descriptor byte count transferred

	NewOS::UInt32 Ctba;	 // Command table descriptor base address
	NewOS::UInt32 Ctbau; // Command table descriptor base address upper 32 bits

	NewOS::UInt32 Reserved1[4]; // Reserved
} HbaCmdHeader;

typedef struct HbaFis final
{
	// 0x00
	FisDmaSetup	 Dsfis; // DMA Setup FIS
	NewOS::UInt8 Pad0[4];
	// 0x20
	FisPioSetup	 Psfis; // PIO Setup FIS
	NewOS::UInt8 Pad1[12];
	// 0x40
	FisRegD2H	 Rfis; // Register – Device to Host FIS
	NewOS::UInt8 Pad2[4];
	// 0x58
	FisDevBits Sdbfis; // Set Device Bit FIS
	// 0x60
	NewOS::UInt8 Ufis[64];
	// 0xA0
	NewOS::UInt8 Rsv[0x100 - 0xA0];
} HbaFis;

typedef struct HbaPrdtEntry final
{
	NewOS::UInt32 Dba;		 // Data base address
	NewOS::UInt32 Dbau;		 // Data base address upper 32 bits
	NewOS::UInt32 Reserved0; // Reserved
	// DW3
	NewOS::UInt32 Dbc : 22;			// Byte count, 4M max
	NewOS::UInt32 Reserved1 : 9;	// Reserved
	NewOS::UInt32 InterruptBit : 1; // Interrupt on completion
} HbaPrdtEntry;

typedef struct HbaCmdTbl final
{
	NewOS::UInt8		Cfis[64];		// Command FIS
	NewOS::UInt8		Acmd[16];		// ATAPI command, 12 or 16 bytes
	NewOS::UInt8		Rsv[48];		// Reserved
	struct HbaPrdtEntry prdtEntries[1]; // Physical region descriptor table entries, 0 ~ 65535
} HbaCmdTbl;

/* EOF */

#if defined(__AHCI__)

/// @brief Initializes an AHCI disk.
/// @param PortsImplemented the amount of port that have been detected.
/// @return
NewOS::Boolean drv_std_init(NewOS::UInt16& PortsImplemented);

NewOS::Boolean drv_std_detected(NewOS::Void);

/// @brief Read from disk.
/// @param Lba
/// @param Buf
/// @param SectorSz
/// @param Size
/// @return
NewOS::Void drv_std_read(NewOS::UInt64 Lba, NewOS::Char* Buf, NewOS::SizeT SectorSz, NewOS::SizeT Size);

/// @brief Write to disk.
/// @param Lba
/// @param Buf
/// @param SectorSz
/// @param Size
/// @return
NewOS::Void drv_std_write(NewOS::UInt64 Lba, NewOS::Char* Buf, NewOS::SizeT SectorSz, NewOS::SizeT Size);

/// @brief get sector count.
NewOS::SizeT drv_std_get_sector_count();

/// @brief get device size.
NewOS::SizeT drv_std_get_drv_size();

/// @brief get sector count.
NewOS::SizeT drv_std_get_sector_count();

/// @brief get device size.
NewOS::SizeT drv_std_get_drv_size();

#endif // ifdef __KERNEL__
