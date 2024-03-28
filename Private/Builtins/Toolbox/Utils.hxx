#ifndef __RSRC_UTIL__
#define __RSRC_UTIL__

// Last Rev
// Sat Feb 24 CET 2024

#define ToolboxInitRsrc() NewOS::SizeT uA = 0

#define kClearClr RGB(00, 00, 00)

#define ToolboxClearRsrc() uA = 0

/// @brief Draws a resource.
#define ToolboxDrawRsrc(ImgPtr, _Height, _Width, BaseX, BaseY) \
  uA = 0;                                                                   \
                                                                            \
  for (NewOS::SizeT i = BaseX; i < _Height + BaseX; ++i) {                  \
    for (NewOS::SizeT u = BaseY; u < _Width + BaseY; ++u) {                 \
      if (ImgPtr[uA] == 0) {                                                \
        *(((volatile NewOS::UInt32*)(kHandoverHeader->f_GOP.f_The +          \
                                     4 *                                    \
                                         kHandoverHeader->f_GOP              \
                                             .f_PixelPerLine *              \
                                         i +                                \
                                     4 * u))) |= ImgPtr[uA];                \
      } else {                                                              \
        *(((volatile NewOS::UInt32*)(kHandoverHeader->f_GOP.f_The +          \
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


/// @brief Cleans a resource.
#define ToolboxClearZone(_Height, _Width, BaseX, BaseY) \
                                                                            \
  for (NewOS::SizeT i = BaseX; i < _Height + BaseX; ++i) {                  \
    for (NewOS::SizeT u = BaseY; u < _Width + BaseY; ++u) {                 \
        *(((volatile NewOS::UInt32*)(kHandoverHeader->f_GOP.f_The +          \
                                     4 *                                    \
                                         kHandoverHeader->f_GOP              \
                                             .f_PixelPerLine *              \
                                         i +                                \
                                     4 * u))) = kClearClr;                 \
    }                                                                         \
  }


/// @brief Draws inside a zone.
#define ToolboxDrawZone(_Clr, _Height, _Width, BaseX, BaseY) \
                                                                            \
  for (NewOS::SizeT i = BaseX; i < _Width + BaseX; ++i) {                  \
    for (NewOS::SizeT u = BaseY; u < _Height + BaseY; ++u) {                 \
        *(((volatile NewOS::UInt32*)(kHandoverHeader->f_GOP.f_The +          \
                                     4 *                                    \
                                         kHandoverHeader->f_GOP              \
                                             .f_PixelPerLine *              \
                                         i +                                \
                                     4 * u))) = _Clr;                 \
    }                                                                         \
  }


#endif
