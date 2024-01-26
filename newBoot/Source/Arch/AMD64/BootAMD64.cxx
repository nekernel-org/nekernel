/*
 *	========================================================
 *
 *	newBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/Boot.hpp>

#define kVGABaseAddress 0xb8000

long long int BStrLen(const char* ptr)
{
    long long int cnt = 0;
    while (*ptr != 0)
    {
        ++ptr;
        ++cnt;
    }

    return cnt;
}

void BKTextWriter::WriteString(
        const char* str,
        unsigned char forecolour,
        unsigned char backcolour,
        int x,
        int y)
{
    if (*str == 0 ||
        !str)
        return;

    for (SizeT idx = 0; idx < BStrLen(str); ++idx)
    {
        this->WriteCharacter(str[idx], forecolour, backcolour, x, y);
        ++x;
    }
}

void BKTextWriter::WriteCharacter(
	            char c,
                    unsigned char forecolour,
                    unsigned char backcolour,
                    int x,
                    int y)
{
    UInt16 attrib = (backcolour << 4) | (forecolour & 0x0F);

    // Video Graphics Array
    // Reads at kVGABaseAddress
    // Decodes UInt16, gets attributes (back colour, fore colour)
    // Gets character, send it to video display with according colour in the registry.

    fWhere = (volatile UInt16*)kVGABaseAddress + (y * 80 + x);
    *fWhere = c | (attrib << 8);
}
