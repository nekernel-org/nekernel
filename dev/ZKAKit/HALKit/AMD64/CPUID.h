/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

	File: CPUID.h
	Purpose: CPUID flags.

	Revision History:

	30/01/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

enum CPUFeatureEnum
{
	kCPUFeatureSSE3		  = 1 << 0,
	kCPUFeaturePCLMUL	  = 1 << 1,
	kCPUFeatureDTES64	  = 1 << 2,
	kCPUFeatureMONITOR	  = 1 << 3,
	kCPUFeatureDS_CPL	  = 1 << 4,
	kCPUFeatureVMX		  = 1 << 5,
	kCPUFeatureSMX		  = 1 << 6,
	kCPUFeatureEST		  = 1 << 7,
	kCPUFeatureTM2		  = 1 << 8,
	kCPUFeatureSSSE3	  = 1 << 9,
	kCPUFeatureCID		  = 1 << 10,
	kCPUFeatureSDBG		  = 1 << 11,
	kCPUFeatureFMA		  = 1 << 12,
	kCPUFeatureCX16		  = 1 << 13,
	kCPUFeatureXTPR		  = 1 << 14,
	kCPUFeaturePDCM		  = 1 << 15,
	kCPUFeaturePCID		  = 1 << 17,
	kCPUFeatureDCA		  = 1 << 18,
	kCPUFeatureSSE4_1	  = 1 << 19,
	kCPUFeatureSSE4_2	  = 1 << 20,
	kCPUFeatureX2APIC	  = 1 << 21,
	kCPUFeatureMOVBE	  = 1 << 22,
	kCPUFeaturePOP3C	  = 1 << 23,
	kCPUFeatureECXTSC	  = 1 << 24,
	kCPUFeatureAES		  = 1 << 25,
	kCPUFeatureXSAVE	  = 1 << 26,
	kCPUFeatureOSXSAVE	  = 1 << 27,
	kCPUFeatureAVX		  = 1 << 28,
	kCPUFeatureF16C		  = 1 << 29,
	kCPUFeatureRDRAND	  = 1 << 30,
	kCPUFeatureHYPERVISOR = 1 << 31,
	kCPUFeatureFPU		  = 1 << 0,
	kCPUFeatureVME		  = 1 << 1,
	kCPUFeatureDE		  = 1 << 2,
	kCPUFeaturePSE		  = 1 << 3,
	kCPUFeatureEDXTSC	  = 1 << 4,
	kCPUFeatureMSR		  = 1 << 5,
	kCPUFeaturePAE		  = 1 << 6,
	kCPUFeatureMCE		  = 1 << 7,
	kCPUFeatureCX8		  = 1 << 8,
	kCPUFeatureAPIC		  = 1 << 9,
	kCPUFeatureSEP		  = 1 << 11,
	kCPUFeatureMTRR		  = 1 << 12,
	kCPUFeaturePGE		  = 1 << 13,
	kCPUFeatureMCA		  = 1 << 14,
	kCPUFeatureCMOV		  = 1 << 15,
	kCPUFeaturePAT		  = 1 << 16,
	kCPUFeaturePSE36	  = 1 << 17,
	kCPUFeaturePSN		  = 1 << 18,
	kCPUFeatureCLFLUSH	  = 1 << 19,
	kCPUFeatureDS		  = 1 << 21,
	kCPUFeatureACPI		  = 1 << 22,
	kCPUFeatureMMX		  = 1 << 23,
	kCPUFeatureFXSR		  = 1 << 24,
	kCPUFeatureSSE		  = 1 << 25,
	kCPUFeatureSSE2		  = 1 << 26,
	kCPUFeatureSS		  = 1 << 27,
	kCPUFeatureHTT		  = 1 << 28,
	kCPUFeatureTM		  = 1 << 29,
	kCPUFeatureIA64		  = 1 << 30,
	kCPUFeaturePBE		  = 1 << 31
};

namespace Kernel
{
	typedef Int64 CPUID;
} // namespace Kernel
