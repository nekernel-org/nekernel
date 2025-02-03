/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	File: BootTextWriter.cc
	Purpose: BootZ string library

	Revision History:



------------------------------------------- */

#include <FirmwareKit/EFI/API.h>
#include <BootKit/Platform.h>
#include <BootKit/Protocol.h>
#include <BootKit/BootKit.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BUGS: 0																							  ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@brief puts wrapper over EFI ConOut.
*/
Boot::BTextWriter& Boot::BTextWriter::Write(const CharacterTypeUTF16* str)
{
#ifdef __DEBUG__
	if (!str || *str == 0)
		return *this;

	CharacterTypeUTF16 strTmp[2];
	strTmp[1] = 0;

	for (size_t i = 0; str[i] != 0; i++)
	{
		if (str[i] == '\r')
		{
			strTmp[0] = str[i];
			ST->ConOut->OutputString(ST->ConOut, strTmp);

			strTmp[0] = '\n';
			ST->ConOut->OutputString(ST->ConOut, strTmp);
		}
		else
		{
			strTmp[0] = str[i];
			ST->ConOut->OutputString(ST->ConOut, strTmp);
		}
	}
#endif // ifdef __DEBUG__

	return *this;
}

/// @brief UTF-8 equivalent of Write (UTF-16).
/// @param str the input string.
Boot::BTextWriter& Boot::BTextWriter::Write(const Char* str)
{
#ifdef __DEBUG__
	if (!str || *str == 0)
		return *this;

	CharacterTypeUTF16 strTmp[2];
	strTmp[1] = 0;

	for (size_t i = 0; str[i] != 0; i++)
	{
		if (str[i] == '\r')
		{
			strTmp[0] = str[i];
			ST->ConOut->OutputString(ST->ConOut, strTmp);

			strTmp[0] = '\n';
			ST->ConOut->OutputString(ST->ConOut, strTmp);
		}
		else
		{
			strTmp[0] = str[i];
			ST->ConOut->OutputString(ST->ConOut, strTmp);
		}
	}
#endif // ifdef __DEBUG__

	return *this;
}

Boot::BTextWriter& Boot::BTextWriter::Write(const UChar* str)
{
#ifdef __DEBUG__
	if (!str || *str == 0)
		return *this;

	CharacterTypeUTF16 strTmp[2];
	strTmp[1] = 0;

	for (size_t i = 0; str[i] != 0; i++)
	{
		if (str[i] == '\r')
		{
			strTmp[0] = str[i];
			ST->ConOut->OutputString(ST->ConOut, strTmp);

			strTmp[0] = '\n';
			ST->ConOut->OutputString(ST->ConOut, strTmp);
		}
		else
		{
			strTmp[0] = str[i];
			ST->ConOut->OutputString(ST->ConOut, strTmp);
		}
	}
#endif // ifdef __DEBUG__

	return *this;
}

/**
@brief putc wrapper over EFI ConOut.
*/
Boot::BTextWriter& Boot::BTextWriter::WriteCharacter(CharacterTypeUTF16 c)
{
#ifdef __DEBUG__
	EfiCharType str[2];

	str[0] = c;
	str[1] = 0;
	ST->ConOut->OutputString(ST->ConOut, str);
#endif // ifdef __DEBUG__

	return *this;
}

Boot::BTextWriter& Boot::BTextWriter::Write(const Long& x)
{
#ifdef __DEBUG__
	this->_Write(x);
	this->Write("h");
#endif // ifdef __DEBUG__

	return *this;
}

Boot::BTextWriter& Boot::BTextWriter::_Write(const Long& x)
{
#ifdef __DEBUG__
	UInt64 y = (x > 0 ? x : -x) / 16;
	UInt64 h = (x > 0 ? x : -x) % 16;

	if (y)
		this->_Write(y);

	/* fail if the hex number is not base-16 */
	if (h > 16)
	{
		this->WriteCharacter('?');
		return *this;
	}

	if (y < 0)
		y = -y;

	const char cNumbers[] = "0123456789ABCDEF";

	this->WriteCharacter(cNumbers[h]);
#endif // ifdef __DEBUG__

	return *this;
}
