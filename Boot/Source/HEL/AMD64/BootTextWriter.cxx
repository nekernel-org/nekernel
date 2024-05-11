/* -------------------------------------------

    Copyright SoftwareLabs

    File: String.cxx
    Purpose: NewBoot string library

    Revision History:



------------------------------------------- */

#include <FirmwareKit/EFI/API.hxx>
#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>
#include <BootKit/BootKit.hxx>

/// BUGS: 0

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@brief puts wrapper over EFI ConOut.
*/
BTextWriter& BTextWriter::Write(const CharacterTypeUTF16* str)
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
BTextWriter& BTextWriter::Write(const Char* str)
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
BTextWriter& BTextWriter::WriteCharacter(CharacterTypeUTF16 c)
{
#ifdef __DEBUG__
	EfiCharType str[2];

	str[0] = c;
	str[1] = 0;
	ST->ConOut->OutputString(ST->ConOut, str);
#endif // ifdef __DEBUG__

	return *this;
}

BTextWriter& BTextWriter::Write(const Long& x)
{
#ifdef __DEBUG__
	this->Write(L"0x");
	this->_Write(x);

#endif // ifdef __DEBUG__

	return *this;
}

BTextWriter& BTextWriter::_Write(const Long& x)
{
#ifdef __DEBUG__
	UInt64 y = (x > 0 ? x : -x) / 16;
	UInt64 h = (x > 0 ? x : -x) % 16;

	if (y)
		this->_Write(y);

	/* fail if the hex number is not base-16 */
	if (h > 15)
	{
		this->WriteCharacter('?');
		return *this;
	}

	if (y < 0)
		y = -y;

	const char NUMBERS[17] = "0123456789ABCDEF";

	this->WriteCharacter(NUMBERS[h]);
#endif // ifdef __DEBUG__

	return *this;
}
