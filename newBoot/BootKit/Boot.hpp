/*
 *	========================================================
 *
 *	newBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>

using namespace hCore;

typedef void* PEFImage;

enum
{
	kSegmentCode = 2,
	kSegmentData = 4,
	kSegmentBss  = 6,
};

class BTextWriter final
{
    volatile UInt16* fWhere;

public:
    void WriteString(const char* c,
                     unsigned char forecolour,
                     unsigned char backcolour,
                     int x,
                     int y);

    void WriteCharacter(char c,
                        unsigned char forecolour,
                        unsigned char backcolour,
                        int x,
                        int y);

public:
    BTextWriter() = default;
    ~BTextWriter() = default;

public:
    BTextWriter& operator=(const BTextWriter&) = default;
    BTextWriter(const BTextWriter&) = default;

};

enum
{
    kBlack,
    kBlue,
    kGreen,
    kCyan,
    kRed,
    kMagenta,
    kBrown,
    kLightGray,
    kDarkGray,
    kLightBlue,
    kLightGreen,
    kLightCyan,
    kLightRed,
    kLightMagenta,
    kYellow,
    kWhite,
};
