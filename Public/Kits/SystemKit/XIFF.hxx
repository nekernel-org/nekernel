/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#ifndef __XIFF__
#define __XIFF__

/** ---------------------------------------------------

    * THIS FILE CONTAINS CODE FOR THE eXtended Information File Format.
    * XIFF is used to make setup programs/audio/video files.

------------------------------------------------------- */

#include <NewKit/Defines.hpp>

/***
 * @brief Generic XIFF header
 * Used by XIFF based containers.
 */

struct PACKED XiffHeader final {
  unsigned char f_Mag[5];      // XIFF string (includes \0)
  unsigned int f_Size;         // overall size of header (XiffHeader) in bytes
  unsigned int f_FormatType;  // format type. generic
  unsigned char f_SpecificMag[4];      // The sub header magic
  unsigned int f_SpecificSize;         // length of the format data
  unsigned int f_SpecificFormatType;  // format type. generic
};

#define kXIFFVideo "XVFF"
#define kXIFFAudio "XAFF"
#define kXIFFInstaller "XnFF"
#define kXIFFGeneric "XIFF"

#endif  // ifndef __XIFF__
