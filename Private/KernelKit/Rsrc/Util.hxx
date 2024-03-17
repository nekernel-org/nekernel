#ifndef __RSRC_UTIL__
#define __RSRC_UTIL__

// Last Rev
// Sat Feb 24 CET 2024

#define KeInitRsrc() HCore::SizeT uA = 0

#define KeClearRsrc() uA = 0

/// @brief draws a resource.
#define KeDrawRsrc(ImgPtr, _Height, _Width, BaseX, BaseY) \
  uA = 0;                                                                   \
                                                                            \
  for (HCore::SizeT i = BaseX; i < _Height + BaseX; ++i) {                  \
    for (HCore::SizeT u = BaseY; u < _Width + BaseY; ++u) {                 \
      if (ImgPtr[uA] == 0) {                                                \
        *(((volatile HCore::UInt32*)(kHandoverHeader->f_GOP.f_The +          \
                                     4 *                                    \
                                         kHandoverHeader->f_GOP              \
                                             .f_PixelPerLine *              \
                                         i +                                \
                                     4 * u))) |= ImgPtr[uA];                \
      } else {                                                              \
        *(((volatile HCore::UInt32*)(kHandoverHeader->f_GOP.f_The +          \
                                     4 *                                    \
                                         kHandoverHeader->f_GOP              \
                                             .f_PixelPerLine *              \
                                         i +                                \
                                     4 * u))) = ImgPtr[uA];                 \
      }                                                                     \
                                                                            \
      ++uA;                                                                 \
    }                                                                       \
  }


/// @brief cleans a resource.
#define KeClearZone(_Height, _Width, BaseX, BaseY) \
                                                                            \
  for (HCore::SizeT i = BaseX; i < _Height + BaseX; ++i) {                  \
    for (HCore::SizeT u = BaseY; u < _Width + BaseY; ++u) {                 \
        *(((volatile HCore::UInt32*)(kHandoverHeader->f_GOP.f_The +          \
                                     4 *                                    \
                                         kHandoverHeader->f_GOP              \
                                             .f_PixelPerLine *              \
                                         i +                                \
                                     4 * u))) = RGB(0, 0, 0);                 \
    }                                                                         \
  }


#define KeDrawZone(_Clr, _Height, _Width, BaseX, BaseY) \
                                                                            \
  for (HCore::SizeT i = BaseX; i < _Height + BaseX; ++i) {                  \
    for (HCore::SizeT u = BaseY; u < _Width + BaseY; ++u) {                 \
        *(((volatile HCore::UInt32*)(kHandoverHeader->f_GOP.f_The +          \
                                     4 *                                    \
                                         kHandoverHeader->f_GOP              \
                                             .f_PixelPerLine *              \
                                         i +                                \
                                     4 * u))) = _Clr;                 \
    }                                                                         \
  }


#endif
