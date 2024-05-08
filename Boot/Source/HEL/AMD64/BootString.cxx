/* -------------------------------------------

    Copyright SoftwareLabs

    File: String.cxx
    Purpose: NewBoot string library

    Revision History:



------------------------------------------- */

#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>
#include <BootKit/BootKit.hxx>

/// bugs 0

/////////////////////////////////////////////////////////////////////////////////////////////////////////

NewOS::SizeT BCopyMem(CharacterTypeUTF16* dest, CharacterTypeUTF16* src, const NewOS::SizeT len)
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

NewOS::SizeT BStrLen(const CharacterTypeUTF16* ptr)
{
	if (!ptr)
		return 0;

	NewOS::SizeT cnt = 0;

	while (*ptr != (CharacterTypeUTF16)0)
	{
		++ptr;
		++cnt;
	}

	return cnt;
}

NewOS::SizeT BSetMem(CharacterTypeUTF16* src, const CharacterTypeUTF16 byte, const NewOS::SizeT len)
{
	if (!src)
		return 0;

	NewOS::SizeT cnt = 0UL;

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

NewOS::SizeT BSetMem(CharacterTypeUTF8* src, const CharacterTypeUTF8 byte, const NewOS::SizeT len)
{
	if (!src)
		return 0;

	NewOS::SizeT cnt = 0UL;

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
