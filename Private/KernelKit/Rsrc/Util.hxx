#ifndef __RSRC_UTIL__
#define __RSRC_UTIL__

// Last Rev
// Sat Feb 24 CET 2024

#define ResourceInit() HCore::SizeT uA = 0
#define ResourceClear() uA = 0

/// @brief draws a resource.
#define DrawResource(ImgPtr, HandoverHeader, _Height, _Width, BaseX, BaseY) \
  uA = 0;                                                                   \
                                                                            \
  for (HCore::SizeT i = BaseX; i < _Height + BaseX; ++i) {                  \
    for (HCore::SizeT u = BaseY; u < _Width + BaseY; ++u) {                 \
      if (ImgPtr[uA] == 0) {                                                \
        *(((volatile HCore::UInt32*)(HandoverHeader->f_GOP.f_The +          \
                                     4 *                                    \
                                         HandoverHeader->f_GOP              \
                                             .f_PixelPerLine *              \
                                         i +                                \
                                     4 * u))) |= ImgPtr[uA];                \
      } else {                                                              \
        *(((volatile HCore::UInt32*)(HandoverHeader->f_GOP.f_The +          \
                                     4 *                                    \
                                         HandoverHeader->f_GOP              \
                                             .f_PixelPerLine *              \
                                         i +                                \
                                     4 * u))) = ImgPtr[uA];                 \
      }                                                                     \
                                                                            \
      ++uA;                                                                 \
    }                                                                       \
  }

#endif
