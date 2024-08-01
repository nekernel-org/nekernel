/* -------------------------------------------

Copyright ZKA Technologies.

File: Types.hxx.
Purpose: System Call types.

------------------------------------------- */

#pragma once

#define IMPORT_CXX extern "C++"
#define IMPORT_C   extern "C"

typedef bool Bool;
typedef void UInt0;

typedef __UINT64_TYPE__ UInt64;
typedef __UINT32_TYPE__ UInt32;
typedef __UINT16_TYPE__ UInt16;
typedef __UINT8_TYPE__	UInt8;

typedef __SIZE_TYPE__ SizeT;

typedef __INT64_TYPE__ SInt64;
typedef __INT32_TYPE__ SInt32;
typedef __INT16_TYPE__ SInt16;
typedef __INT8_TYPE__  SInt8;

typedef void*			 VoidPtr;
typedef __UINTPTR_TYPE__ UIntPtr;
typedef char			 Char;

inline constexpr auto cSerialAlertSyscall	 = 0x10;
inline constexpr auto cTlsSyscall			 = 0x11;
inline constexpr auto cTlsInstallSyscall	 = 0x12;
inline constexpr auto cNewSyscall			 = 0x13;
inline constexpr auto cDeleteSyscall		 = 0x14;
inline constexpr auto cExitSyscall			 = 0x15;
inline constexpr auto cLastExitSyscall		 = 0x16;
inline constexpr auto cCatalogOpenSyscall	 = 0x17;
inline constexpr auto cForkReadSyscall		 = 0x18;
inline constexpr auto cForkWriteSyscall		 = 0x19;
inline constexpr auto cCatalogCloseSyscall	 = 0x20;
inline constexpr auto cCatalogRemoveSyscall	 = 0x21;
inline constexpr auto cCatalogCreateSyscall	 = 0x22;
inline constexpr auto cRebootSyscallSyscall	 = 0x23;
inline constexpr auto cShutdownSyscall		 = 0x24;
inline constexpr auto cLPCSendMsgSyscall	 = 0x25;
inline constexpr auto cLPCOpenMsgSyscall	 = 0x26;
inline constexpr auto cLPCCloseMsgSyscall	 = 0x27;
inline constexpr auto cLPCSanitizeMsgSyscall = 0x28;

#ifdef __NEWOS_SYMS__
#include <SCIKit/rtl.internal.inl>
#else
class UnknownInterface; // Refrenced from an IDB entry.
class UnknownUCLSID;	// From the IDB, the constructor of the object, e.g: WordUCLSID.
class UUID;

/// @brief Allocate new SCM class.
/// @tparam TCLS
/// @tparam UCLSID
/// @param uclsidOfCls
/// @return
template <typename TCLS, typename UCLSID, typename... Args>
TCLS* RtlQueryInterface(UCLSID uclsidOfCls, Args... args);

/// @brief Release SCM class.
/// @tparam TCLS
/// @param cls
/// @return
template <typename TCLS>
SInt32 RtlReleaseClass(TCLS* cls);

class __attribute__((uuid("d7c144b6-0792-44b8-b06b-02b227b547df"))) UnknownInterface
{
public:
	explicit UnknownInterface() = default;
	virtual ~UnknownInterface() = default;

	UnknownInterface& operator=(const UnknownInterface&) = default;
	UnknownInterface(const UnknownInterface&)			 = default;

	virtual SInt32			  Release()					   = 0;
	virtual void			  DecrementRef()			   = 0;
	virtual UnknownInterface* IncrementRef()			   = 0;
	virtual VoidPtr			  QueryInterface(UUID* p_uuid) = 0;
};

#endif

/* ================================================ */
/* SCI structures */
/* ================================================ */

struct HEAP_ALLOC_INFO final
{
	VoidPtr fThe;
	SizeT	fTheSz;
};

struct THREAD_INFORMATION_BLOCK final
{
	Char	f_Cookie[3]; // Process cookie.
	UIntPtr f_Code;		 // Start Address
	UIntPtr f_Data;		 // Allocation Heap
	UIntPtr f_BSS;		 // Stack Pointer.
	SInt32	f_ID;		 // Thread execution ID.
};

struct PROCESS_BLOCK_INFO final
{
	THREAD_INFORMATION_BLOCK* fTIB;
	THREAD_INFORMATION_BLOCK* fGIB;
};

struct PROCESS_EXIT_INFO final
{
	static constexpr auto cReasonLen = 512;

	SInt64 fCode;
	Char   fReason[cReasonLen];
};

/// @brief Raise system call.
/// @param id the system call id could be 0x10 for example.
/// @param data the data associated with it.
/// @param data_sz the size of the data associated with it.
/// @return status code.
IMPORT_C SInt32 RtlRaiseSystemCall(const SInt32 id, VoidPtr data, SizeT data_sz);

IMPORT_C VoidPtr RtlGetDLLProcedure(const char* symbol, VoidPtr dll_handle);
IMPORT_C VoidPtr RtOpenDLL(const char* path);
IMPORT_C void RtCloseDLL(VoidPtr dll_handle);