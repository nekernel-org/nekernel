/* -------------------------------------------

    Copyright Mahrouss Logic

    File: CPUID.hxx
    Purpose: CPUID flags.

    Revision History:

    30/01/24: Added file (amlel)

------------------------------------------- */

#pragma once

enum
{
	CPU_FEATURE_ECX_SSE3	   = 1 << 0,
	CPU_FEATURE_ECX_PCLMUL	   = 1 << 1,
	CPU_FEATURE_ECX_DTES64	   = 1 << 2,
	CPU_FEATURE_ECX_MONITOR	   = 1 << 3,
	CPU_FEATURE_ECX_DS_CPL	   = 1 << 4,
	CPU_FEATURE_ECX_VMX		   = 1 << 5,
	CPU_FEATURE_ECX_SMX		   = 1 << 6,
	CPU_FEATURE_ECX_EST		   = 1 << 7,
	CPU_FEATURE_ECX_TM2		   = 1 << 8,
	CPU_FEATURE_ECX_SSSE3	   = 1 << 9,
	CPU_FEATURE_ECX_CID		   = 1 << 10,
	CPU_FEATURE_ECX_SDBG	   = 1 << 11,
	CPU_FEATURE_ECX_FMA		   = 1 << 12,
	CPU_FEATURE_ECX_CX16	   = 1 << 13,
	CPU_FEATURE_ECX_XTPR	   = 1 << 14,
	CPU_FEATURE_ECX_PDCM	   = 1 << 15,
	CPU_FEATURE_ECX_PCID	   = 1 << 17,
	CPU_FEATURE_ECX_DCA		   = 1 << 18,
	CPU_FEATURE_ECX_SSE4_1	   = 1 << 19,
	CPU_FEATURE_ECX_SSE4_2	   = 1 << 20,
	CPU_FEATURE_ECX_X2APIC	   = 1 << 21,
	CPU_FEATURE_ECX_MOVBE	   = 1 << 22,
	CPU_FEATURE_ECX_POP3C	   = 1 << 23,
	CPU_FEATURE_ECX_TSC		   = 1 << 24,
	CPU_FEATURE_ECX_AES		   = 1 << 25,
	CPU_FEATURE_ECX_XSAVE	   = 1 << 26,
	CPU_FEATURE_ECX_OSXSAVE	   = 1 << 27,
	CPU_FEATURE_ECX_AVX		   = 1 << 28,
	CPU_FEATURE_ECX_F16C	   = 1 << 29,
	CPU_FEATURE_ECX_RDRAND	   = 1 << 30,
	CPU_FEATURE_ECX_HYPERVISOR = 1 << 31,
	CPU_FEATURE_EDX_FPU		   = 1 << 0,
	CPU_FEATURE_EDX_VME		   = 1 << 1,
	CPU_FEATURE_EDX_DE		   = 1 << 2,
	CPU_FEATURE_EDX_PSE		   = 1 << 3,
	CPU_FEATURE_EDX_TSC		   = 1 << 4,
	CPU_FEATURE_EDX_MSR		   = 1 << 5,
	CPU_FEATURE_EDX_PAE		   = 1 << 6,
	CPU_FEATURE_EDX_MCE		   = 1 << 7,
	CPU_FEATURE_EDX_CX8		   = 1 << 8,
	CPU_FEATURE_EDX_APIC	   = 1 << 9,
	CPU_FEATURE_EDX_SEP		   = 1 << 11,
	CPU_FEATURE_EDX_MTRR	   = 1 << 12,
	CPU_FEATURE_EDX_PGE		   = 1 << 13,
	CPU_FEATURE_EDX_MCA		   = 1 << 14,
	CPU_FEATURE_EDX_CMOV	   = 1 << 15,
	CPU_FEATURE_EDX_PAT		   = 1 << 16,
	CPU_FEATURE_EDX_PSE36	   = 1 << 17,
	CPU_FEATURE_EDX_PSN		   = 1 << 18,
	CPU_FEATURE_EDX_CLFLUSH	   = 1 << 19,
	CPU_FEATURE_EDX_DS		   = 1 << 21,
	CPU_FEATURE_EDX_ACPI	   = 1 << 22,
	CPU_FEATURE_EDX_MMX		   = 1 << 23,
	CPU_FEATURE_EDX_FXSR	   = 1 << 24,
	CPU_FEATURE_EDX_SSE		   = 1 << 25,
	CPU_FEATURE_EDX_SSE2	   = 1 << 26,
	CPU_FEATURE_EDX_SS		   = 1 << 27,
	CPU_FEATURE_EDX_HTT		   = 1 << 28,
	CPU_FEATURE_EDX_TM		   = 1 << 29,
	CPU_FEATURE_EDX_IA64	   = 1 << 30,
	CPU_FEATURE_EDX_PBE		   = 1 << 31
};

typedef int CPU_FEATURE;