/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <EncryptFS/disk_crypt.hxx>

enum
{
	kEFSFlagEncryptedFree	  = 0x00,
	kEFSFlagEncryptedUnlocked = 0x3f,
	kEFSFlagEncryptedLock	  = 0xf3,
};
