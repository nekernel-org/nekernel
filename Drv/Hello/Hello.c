/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>
#include <DDK/KernelDev.h>

struct HelloMasterConfigHeader;
struct HelloMasterConfigHeaderInput;

struct HelloMasterConfigHeaderInput
{
	size_t fSizeOfOutput;
	struct HelloMasterConfigHeader* fOutput;
};


/// @file Hello.c
/// @brief Zero configuration protocol, a much more better protocol for zero configuration.

#define cHMCHMaxDataLength (1024)

typedef struct HelloMasterConfigHeader
{
	int64_t fMagic;
	int64_t fVersion;
	int64_t fSourceAddress;
	size_t  fDataLength;
	wchar_t fUTF16Data[cHMCHMaxDataLength];
} __attribute__((packed)) HelloMasterConfigHeader;

#define cHMCHDeviceLen 255

static kernelDeviceRef cDev = nil;
static char* cDeviceUUID = nil; // 3ed40738-c7d6-4b59-afdf-3c104b05fbf
static HelloMasterConfigHeader* cHeader = nil;

/// @brief Link to master device to attribute DHCP lease.
static void __hello_link_device(void* a0)
{
	kernelPrintStr("Hello: turning up...\r");

	if (!cDev)
	{
		// open raw network device.
		cDev = kernelOpenDevice("RAWNET:\\");
	}

	cDev->write("+LINK", kernelStringLength("+LINK"));
	cDev->wait();

	cDev->write((void*)cDeviceUUID, kernelStringLength(cDeviceUUID));
	cDev->wait();

	if (cHeader)
	{
		kernelFree(cHeader);
		cHeader = nil;
	}

	cHeader = cDev->read(nil, sizeof(HelloMasterConfigHeader));
}

static void __hello_get_hmch(void* a0)
{
	if (a0 == nil) return;

	kernelPrintStr("Hello: returning header...\r");

	struct HelloMasterConfigHeaderInput* in = a0;
	in->fOutput = cHeader;
	in->fSizeOfOutput = sizeof(in->fOutput);
}

static void __hello_unlink_device(void* a0)
{
	kernelPrintStr("Hello: shutting down...\r");

	if (cDev)
	{
		cDev->write("+UNLINK", kernelStringLength("+UNLINK"));
		cDev->wait();

		/// here is my uuid and my config header. Please disconnect me.
		cDev->write((void*)cDeviceUUID, kernelStringLength(cDeviceUUID));
		cDev->write(cHeader, sizeof(cHeader));

		cDev->wait();

		kernelCloseDevice(cDev);
		cDev = nil;
	}

	if (cDeviceUUID)
	{
		kernelFree((void*)cDeviceUUID);
		cDeviceUUID = nil;
	}

	cDev = nil;
}

int __at_enter(void)
{
	kernelPrintStr("Hello: starting up Helloconf...\r");

	cDeviceUUID = kernelAlloc(sizeof(char) * cHMCHDeviceLen);

	kernelAddSyscall(0, __hello_link_device);
	kernelAddSyscall(1, __hello_unlink_device);
	kernelAddSyscall(2, __hello_get_hmch);

	return 0;
}

int __at_exit(void)
{
	kernelPrintStr("Hello: starting up Helloconf...\r");

	// first unlink.
	__hello_unlink_device(nil);

	// then unregister syscalls.
	kernelAddSyscall(0, nil);
	kernelAddSyscall(1, nil);

	return 0;
}
