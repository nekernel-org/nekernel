/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Defines.hxx
    Purpose: AHCI header.

    Revision History:

    03/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

// Forward declarations of structs.

struct HbaPort;
struct FisData;
struct FisRegD2H;
struct FisRegH2D;

// Enum types

typedef enum {
  FIS_TYPE_REG_H2D = 0x27,    // Register FIS - host to device
  FIS_TYPE_REG_D2H = 0x34,    // Register FIS - device to host
  FIS_TYPE_DMA_ACT = 0x39,    // DMA activate FIS - device to host
  FIS_TYPE_DMA_SETUP = 0x41,  // DMA setup FIS - bidirectional
  FIS_TYPE_DATA = 0x46,       // Data FIS - bidirectional
  FIS_TYPE_BIST = 0x58,       // BIST activate FIS - bidirectional
  FIS_TYPE_PIO_SETUP = 0x5F,  // PIO setup FIS - device to host
  FIS_TYPE_DEV_BITS = 0xA1,   // Set device bits FIS - device to host
} AHCI_FIS_TYPE;

typedef enum {
  AHCI_ATA_CMD_IDENTIFY = 0xEC,
  AHCI_ATA_CMD_READ_DMA = 0xC8,
  AHCI_ATA_CMD_READ_DMA_EX = 0x25,
  AHCI_ATA_CMD_WRITE_DMA = 0xCA,
  AHCI_ATA_CMD_WRITE_DMA_EX = 0x35
} AHCI_FIS_COMMAND;

typedef struct FisRegH2D final {
  // DWORD 0
  HCore::UInt8 fisType;  // FIS_TYPE_REG_H2D

  HCore::UInt8 portMul : 4;    // Port multiplier
  HCore::UInt8 reserved0 : 3;  // Reserved
  HCore::UInt8 cmdOrCtrl : 1;  // 1: Command, 0: Control

  HCore::UInt8 command;   // Command register
  HCore::UInt8 featurel;  // Feature register, 7:0

  // DWORD 1
  HCore::UInt8 lba0;    // LBA low register, 7:0
  HCore::UInt8 lba1;    // LBA mid register, 15:8
  HCore::UInt8 lba2;    // LBA high register, 23:16
  HCore::UInt8 device;  // Device register

  // DWORD 2
  HCore::UInt8 lba3;         // LBA register, 31:24
  HCore::UInt8 lba4;         // LBA register, 39:32
  HCore::UInt8 lba5;         // LBA register, 47:40
  HCore::UInt8 featureHigh;  // Feature register, 15:8

  // DWORD 3
  HCore::UInt8 countLow;   // Count register, 7:0
  HCore::UInt8 countHigh;  // Count register, 15:8
  HCore::UInt8 icc;        // Isochronous command completion
  HCore::UInt8 control;    // Control register

  // DWORD 4
  HCore::UInt8 reserved1[4];  // Reserved
} FisRegH2D;

typedef struct FisRegD2H final {
  // DWORD 0
  HCore::UInt8 fisType;  // FIS_TYPE_REG_D2H

  HCore::UInt8 portMul : 4;       // Port multiplier
  HCore::UInt8 reserved0 : 2;     // Reserved
  HCore::UInt8 interruptBit : 1;  // Interrupt bit
  HCore::UInt8 reserved1 : 1;     // Reserved

  HCore::UInt8 status;  // Status register
  HCore::UInt8 error;   // Error register

  // DWORD 1
  HCore::UInt8 lba0;    // LBA low register, 7:0
  HCore::UInt8 lba1;    // LBA mid register, 15:8
  HCore::UInt8 lba2;    // LBA high register, 23:16
  HCore::UInt8 device;  // Device register

  // DWORD 2
  HCore::UInt8 lba3;  // LBA register, 31:24
  HCore::UInt8 lba4;  // LBA register, 39:32
  HCore::UInt8 lba5;  // LBA register, 47:40
  HCore::UInt8 rsv2;  // Reserved

  // DWORD 3
  HCore::UInt8 countLow;   // Count register, 7:0
  HCore::UInt8 countHigh;  // Count register, 15:8
  HCore::UInt8 rsv3[2];    // Reserved

  // DWORD 4
  HCore::UInt8 rsv4[4];  // Reserved
} FisRegD2H;

typedef struct FisData final {
  // DWORD 0
  HCore::UInt8 fisType;  // FIS_TYPE_DATA

  HCore::UInt8 portMul : 4;    // Port multiplier
  HCore::UInt8 reserved0 : 4;  // Reserved

  HCore::UInt8 reserved1[2];  // Reserved

  // DWORD 1 ~ N
  HCore::UInt32 data[1];  // Payload
} FisData;

typedef struct FisPioSetup final {
  // DWORD 0
  HCore::UInt8 fisType;  // FIS_TYPE_PIO_SETUP

  HCore::UInt8 portMul : 4;       // Port multiplier
  HCore::UInt8 reserved0 : 1;     // Reserved
  HCore::UInt8 d : 1;             // Data transfer direction, 1 - device to host
  HCore::UInt8 interruptBit : 1;  // Interrupt bit
  HCore::UInt8 reserved1 : 1;

  HCore::UInt8 status;  // Status register
  HCore::UInt8 error;   // Error register

  // DWORD 1
  HCore::UInt8 lba0;    // LBA low register, 7:0
  HCore::UInt8 lba1;    // LBA mid register, 15:8
  HCore::UInt8 lba2;    // LBA high register, 23:16
  HCore::UInt8 device;  // Device register

  // DWORD 2
  HCore::UInt8 lba3;  // LBA register, 31:24
  HCore::UInt8 lba4;  // LBA register, 39:32
  HCore::UInt8 lba5;  // LBA register, 47:40
  HCore::UInt8 rsv2;  // Reserved

  // DWORD 3
  HCore::UInt8 countLow;   // Count register, 7:0
  HCore::UInt8 countHigh;  // Count register, 15:8
  HCore::UInt8 rsv3;       // Reserved
  HCore::UInt8 eStatus;    // New value of status register

  // DWORD 4
  HCore::UInt16 tc;      // Transfer count
  HCore::UInt8 rsv4[2];  // Reserved
} FisPioSetup;

typedef struct FisDmaSetup final {
  // DWORD 0
  HCore::UInt8 fisType;  // FIS_TYPE_DMA_SETUP

  HCore::UInt8 portMul : 4;       // Port multiplier
  HCore::UInt8 reserved0 : 1;     // Reserved
  HCore::UInt8 dtd : 1;           // Data transfer direction, 1 - device to host
  HCore::UInt8 interruptBit : 1;  // Interrupt bit
  HCore::UInt8
      autoEnable : 1;  // Auto-activate. Specifies if DMA Activate FIS is needed

  HCore::UInt8 reserved1[2];  // Reserved

  // DWORD 1&2
  HCore::UInt64 dmaBufferId;  // DMA Buffer Identifier. Used to Identify DMA buffer in
                       // host memory. SATA Spec says host specific and not in
                       // Spec. Trying AHCI spec might work.

  // DWORD 3
  HCore::UInt32 rsvd;  // More reserved

  // DWORD 4
  HCore::UInt32 dmabufOffset;  // Byte offset into buffer. First 2 bits must be 0

  // DWORD 5
  HCore::UInt32 transferCount;  // Number of bytes to transfer. Bit 0 must be 0

  // DWORD 6
  HCore::UInt32 reserved3;  // Reserved
} FisDmaSetup;

typedef struct FisDevBits final {
  // DWORD 0
  HCore::UInt8 fisType;  // FIS_TYPE_DMA_SETUP (A1h)

  HCore::UInt8 reserved0 : 5;  // Reserved
  HCore::UInt8 r0 : 1;
  HCore::UInt8 interruptBit : 1;
  HCore::UInt8 n : 1;

  HCore::UInt8 statusLow : 3;
  HCore::UInt8 r1 : 1;
  HCore::UInt8 statusHigh : 3;

  HCore::UInt8 r2 : 1;
  HCore::UInt8 error;

  // DWORD 1
  HCore::UInt32 act;
} FisDevBits;

/// \brief Enable AHCI device bit in GHC register.
#ifndef kAhciGHC_AE
#define kAhciGHC_AE (31)
#endif  //! ifndef kAhciGHC_AE

typedef struct HbaPort final {
  HCore::UInt32 clb;            // 0x00, command list base address, 1K-byte aligned
  HCore::UInt32 clbu;           // 0x04, command list base address upper 32 bits
  HCore::UInt32 fb;             // 0x08, FIS base address, 256-byte aligned
  HCore::UInt32 fbu;            // 0x0C, FIS base address upper 32 bits
  HCore::UInt32 is;             // 0x10, interrupt status
  HCore::UInt32 ie;             // 0x14, interrupt enable
  HCore::UInt32 cmd;            // 0x18, command and status
  HCore::UInt32 reserved0;      // 0x1C, Reserved
  HCore::UInt32 tfd;            // 0x20, task file data
  HCore::UInt32 sig;            // 0x24, signature
  HCore::UInt32 ssts;           // 0x28, SATA status (SCR0:SStatus)
  HCore::UInt32 sctl;           // 0x2C, SATA control (SCR2:SControl)
  HCore::UInt32 serr;           // 0x30, SATA error (SCR1:SError)
  HCore::UInt32 sact;           // 0x34, SATA active (SCR3:SActive)
  HCore::UInt32 ci;             // 0x38, command issue
  HCore::UInt32 sntf;           // 0x20, SATA notification (SCR4:SNotification)
  HCore::UInt32 fbs;            // 0x40, FIS-based switch control
  HCore::UInt32 reserved1[11];  // 0x44 ~ 0x6F, Reserved
  HCore::UInt32 vendor[4];      // 0x70 ~ 0x7F, vendor specific
} HbaPort;

typedef struct HbaMem final {
  // 0x00 - 0x2B, Generic Host Control
  HCore::UInt32 cap;      // 0x00, Host capability
  HCore::UInt32 ghc;      // 0x04, Global host control
  HCore::UInt32 is;       // 0x08, Interrupt status
  HCore::UInt32 pi;       // 0x0C, Port implemented
  HCore::UInt32 vs;       // 0x10, Version
  HCore::UInt32 ccc_ctl;  // 0x14, Command completion coalescing control
  HCore::UInt32 ccc_pts;  // 0x18, Command completion coalescing ports
  HCore::UInt32 em_loc;   // 0x1C, Enclosure management location
  HCore::UInt32 em_ctl;   // 0x20, Enclosure management control
  HCore::UInt32 cap2;     // 0x24, Host capabilities extended
  HCore::UInt32 bohc;     // 0x28, BIOS/OS handoff control and status

  HCore::UInt16 rsv;
  HCore::UInt32 resv2;

  HbaPort ports[1];  // 1 ~ 32
} HbaMem;

typedef struct HbaCmdHeader final {
  // DW0
  HCore::UInt8 cfl : 5;           // Command FIS length in DWORDS, 2 ~ 16
  HCore::UInt8 atapi : 1;         // ATAPI
  HCore::UInt8 write : 1;         // Write, 1: H2D, 0: D2H
  HCore::UInt8 prefetchable : 1;  // Prefetchable

  HCore::UInt8 reset : 1;      // Reset
  HCore::UInt8 BIST : 1;       // BIST
  HCore::UInt8 clear : 1;      // Clear busy upon R_OK
  HCore::UInt8 reserved0 : 1;  // Reserved
  HCore::UInt8 pmp : 4;        // Port multiplier port

  HCore::UInt16 prdtl;           // Physical region descriptor table length in entries
  volatile HCore::UInt32 prdbc;  // Physical region descriptor byte count transferred

  HCore::UInt32 ctba;   // Command table descriptor base address
  HCore::UInt32 ctbau;  // Command table descriptor base address upper 32 bits

  HCore::UInt32 reserved1[4];  // Reserved
} HbaCmdHeader;

typedef struct HbaFis final {
  // 0x00
  FisDmaSetup dsfis;  // DMA Setup FIS
  HCore::UInt8 pad0[4];
  // 0x20
  FisPioSetup psfis;  // PIO Setup FIS
  HCore::UInt8 pad1[12];
  // 0x40
  FisRegD2H rfis;  // Register – Device to Host FIS
  HCore::UInt8 pad2[4];
  // 0x58
  FisDevBits sdbfis;  // Set Device Bit FIS
  // 0x60
  HCore::UInt8 ufis[64];
  // 0xA0
  HCore::UInt8 rsv[0x100 - 0xA0];
} HbaFis;

typedef struct HbaPrdtEntry final {
  HCore::UInt32 dba;        // Data base address
  HCore::UInt32 dbau;       // Data base address upper 32 bits
  HCore::UInt32 reserved0;  // Reserved
  // DW3
  HCore::UInt32 dbc : 22;          // Byte count, 4M max
  HCore::UInt32 reserved1 : 9;     // Reserved
  HCore::UInt32 interruptBit : 1;  // Interrupt on completion
} HbaPrdtEntry;

typedef struct HbaCmdTbl final {
  HCore::UInt8 cfis[64];  // Command FIS
  HCore::UInt8 acmd[16];  // ATAPI command, 12 or 16 bytes
  HCore::UInt8 rsv[48];   // Reserved
  HbaPrdtEntry prdtEntries[1];  // Physical region descriptor table entries, 0 ~ 65535
} HbaCmdTbl;

/* EOF */
