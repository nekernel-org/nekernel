/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#ifndef __ODF__
#define __ODF__

#include <System.Core/Headers/Defs.hxx>

/**
 * @brief Open Document Format
 * @file ODF.hxx
*/

/// @brief four-character code for ODF.
#define kFourCCLength_ODF 4

/// @brief Document file header.
typedef struct ODFFileHeader
{
    CharacterType f_Ident[kFourCCLength_ODF];
    
    Int32Type f_DocumentKind;
    Int32Type f_DocumentSize;

    Int64Type f_MetaForkOffset;
    Int64Type f_DocumentForkOffset;

    CharacterType f_Padding[4];
} PACKED ODFFileHeader;

/// @brief ODF Fork header
typedef struct ODFForkHeader
{
    CharacterType f_MetadataName[255];

    Int32Type f_MetadataKind;
    Int32Type f_MetadataSize;

    CharacterType f_Padding;
} PACKED ODFForkHeader;

#endif // !__ODF__