/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#ifndef __XIFF__
#define __XIFF__

/** ---------------------------------------------------

    * THIS FILE CONTAINS CODE FOR THE eXtended Information File Format.
    * XIFF is used to make setup programs/audio/video files.

------------------------------------------------------- */

#include <System.Core/CoreAPI.hxx>

/// @brief four-character code for XIFF.
#define kFourCCLength_XIFF 4

/***
 * @brief Generic XIFF header
 * Used by XIFF based containers.
 */

struct PACKED XiffHeader final {
  BYTE f_Mag[kFourCCLength_XIFF];      // XIFF string (includes \0)
  DWORD f_Size;         // overall size of header (XiffHeader) in bytes
  DWORD f_FormatType;  // format type. generic
  BYTE f_SpecificMag[4];      // The sub header magic
  DWORD f_SpecificSize;         // length of the format data
  DWORD f_SpecificFormatType;  // format type. generic
};

#define kXIFFContainerVideo "XVFF"
#define kXIFFContainerAudio "XAFF"
#define kXIFFContainerInstaller "XnFF"
#define kXIFFContainerGeneric "XIFF"
#define kXIFFContainerBinary "XEFF"

#endif  // ifndef __XIFF__
