/* -------------------------------------------

	Copyright ZKA Technologies.

	File: String.cxx
	Purpose: NewBoot string library

	Revision History:



------------------------------------------- */

#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>
#include <BootKit/BootKit.hxx>

/// bugs 0

/////////////////////////////////////////////////////////////////////////////////////////////////////////

Kernel::SizeT BCopyMem(CharacterTypeUTF16* dest, CharacterTypeUTF16* src, const Kernel::SizeT len)
{
	if (!dest || !src)
		return 0;

	SizeT index = 0UL;
	for (; index < len; ++index)
	{
		dest[index] = src[index];
	}

	return index;
}

Kernel::SizeT BStrLen(const CharacterTypeUTF16* ptr)
{
	if (!ptr)
		return 0;

	Kernel::SizeT cnt = 0;

	while (*ptr != (CharacterTypeUTF16)0)
	{
		++ptr;
		++cnt;
	}

	return cnt;
}

Kernel::SizeT BSetMem(CharacterTypeUTF16* src, const CharacterTypeUTF16 byte, const Kernel::SizeT len)
{
	if (!src)
		return 0;

	Kernel::SizeT cnt = 0UL;

	while (*src != 0)
	{
		if (cnt > len)
			break;

		*src = byte;
		++src;

		++cnt;
	}

	return cnt;
}

Kernel::SizeT BSetMem(CharacterTypeUTF8* src, const CharacterTypeUTF8 byte, const Kernel::SizeT len)
{
	if (!src)
		return 0;

	Kernel::SizeT cnt = 0UL;

	while (*src != 0)
	{
		if (cnt > len)
			break;

		*src = byte;
		++src;

		++cnt;
	}

	return cnt;
}
