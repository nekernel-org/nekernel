/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#ifndef __ODF__
#define __ODF__

#include <System.Core/Defs.hxx>

/**
 * @brief Open Document Format
 * @file ODF.hxx
*/

/// @brief four-character code for ODF.
#define kFourCCLength_ODF 4

/// @brief Document file header.
typedef struct ODFFileHeader
{
    CHAR f_Ident[kFourCCLength_ODF];
    
    INT32 f_DocumentKind;
    INT32 f_DocumentSize;

    INT64 f_MetaForkOffset;
    INT64 f_DocumentForkOffset;

    CHAR f_Padding[4];
} PACKED ODFFileHeader;

/// @brief ODF Fork header
typedef struct ODFForkHeader
{
    CHAR f_MetadataName[255];

    INT32 f_MetadataKind;
    INT32 f_MetadataSize;

    CHAR f_Padding;
} PACKED ODFForkHeader;

#endif // !__ODF__