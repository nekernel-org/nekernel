/* -------------------------------------------

	Copyright ZKA Web Services Co.

	File: CPUID.hxx
	Purpose: CPUID flags.

	Revision History:

	30/01/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>

enum
{
	eCPUFeatureSSE3		  = 1 << 0,
	eCPUFeaturePCLMUL	  = 1 << 1,
	eCPUFeatureDTES64	  = 1 << 2,
	eCPUFeatureMONITOR	  = 1 << 3,
	eCPUFeatureDS_CPL	  = 1 << 4,
	eCPUFeatureVMX		  = 1 << 5,
	eCPUFeatureSMX		  = 1 << 6,
	eCPUFeatureEST		  = 1 << 7,
	eCPUFeatureTM2		  = 1 << 8,
	eCPUFeatureSSSE3	  = 1 << 9,
	eCPUFeatureCID		  = 1 << 10,
	eCPUFeatureSDBG		  = 1 << 11,
	eCPUFeatureFMA		  = 1 << 12,
	eCPUFeatureCX16		  = 1 << 13,
	eCPUFeatureXTPR		  = 1 << 14,
	eCPUFeaturePDCM		  = 1 << 15,
	eCPUFeaturePCID		  = 1 << 17,
	eCPUFeatureDCA		  = 1 << 18,
	eCPUFeatureSSE4_1	  = 1 << 19,
	eCPUFeatureSSE4_2	  = 1 << 20,
	eCPUFeatureX2APIC	  = 1 << 21,
	eCPUFeatureMOVBE	  = 1 << 22,
	eCPUFeaturePOP3C	  = 1 << 23,
	eCPUFeatureECXTSC	  = 1 << 24,
	eCPUFeatureAES		  = 1 << 25,
	eCPUFeatureXSAVE	  = 1 << 26,
	eCPUFeatureOSXSAVE	  = 1 << 27,
	eCPUFeatureAVX		  = 1 << 28,
	eCPUFeatureF16C		  = 1 << 29,
	eCPUFeatureRDRAND	  = 1 << 30,
	eCPUFeatureHYPERVISOR = 1 << 31,
	eCPUFeatureFPU		  = 1 << 0,
	eCPUFeatureVME		  = 1 << 1,
	eCPUFeatureDE		  = 1 << 2,
	eCPUFeaturePSE		  = 1 << 3,
	eCPUFeatureEDXTSC	  = 1 << 4,
	eCPUFeatureMSR		  = 1 << 5,
	eCPUFeaturePAE		  = 1 << 6,
	eCPUFeatureMCE		  = 1 << 7,
	eCPUFeatureCX8		  = 1 << 8,
	eCPUFeatureAPIC		  = 1 << 9,
	eCPUFeatureSEP		  = 1 << 11,
	eCPUFeatureMTRR		  = 1 << 12,
	eCPUFeaturePGE		  = 1 << 13,
	eCPUFeatureMCA		  = 1 << 14,
	eCPUFeatureCMOV		  = 1 << 15,
	eCPUFeaturePAT		  = 1 << 16,
	eCPUFeaturePSE36	  = 1 << 17,
	eCPUFeaturePSN		  = 1 << 18,
	eCPUFeatureCLFLUSH	  = 1 << 19,
	eCPUFeatureDS		  = 1 << 21,
	eCPUFeatureACPI		  = 1 << 22,
	eCPUFeatureMMX		  = 1 << 23,
	eCPUFeatureFXSR		  = 1 << 24,
	eCPUFeatureSSE		  = 1 << 25,
	eCPUFeatureSSE2		  = 1 << 26,
	eCPUFeatureSS		  = 1 << 27,
	eCPUFeatureHTT		  = 1 << 28,
	eCPUFeatureTM		  = 1 << 29,
	eCPUFeatureIA64		  = 1 << 30,
	eCPUFeaturePBE		  = 1 << 31
};

typedef Kernel::Int64 hal_cpu_feature_type;
