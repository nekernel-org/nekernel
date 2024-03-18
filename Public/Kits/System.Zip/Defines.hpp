/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <System.Core/System.hxx>

#define ZIPKIT_VERSION "1.01"

/// @brief Zip compression function
/// @param dest 
/// @param destLen 
/// @param source 
/// @param sourceLen 
/// @return 
CA_EXTERN_C DWORD HcCompress(BYTE *dest, QWORD *destLen, const BYTE *source,
                             QWORD sourceLen);

/// @brief Zip uncompression function
/// @param dest 
/// @param destLen 
/// @param source 
/// @param sourceLen 
/// @return 
CA_EXTERN_C DWORD HcUncompress(BYTE *dest, QWORD *destLen,
                               const BYTE *source, QWORD sourceLen);

