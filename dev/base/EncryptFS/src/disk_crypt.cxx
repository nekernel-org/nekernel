/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <EncryptFS/disk_crypt.hxx>

enum
{
	eEFSFlagEncryptedFree	= 0x00,
	eEFSFlagEncryptedUnlock = 0x3f,
	eEFSFlagEncryptedLock	= 0xf3,
};
