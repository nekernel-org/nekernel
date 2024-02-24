#ifndef __RSRC_UTIL__
#define __RSRC_UTIL__

/// @brief draws a resource icon.
#define DrawResource(ImgPtr, HandoverHeader, Width, Height, BaseX, BaseY) \
  HCore::SizeT uA = 0;                                                    \
                                                                          \
  for (HCore::SizeT i = BaseX; i < Width + BaseX; ++i) {                  \
    for (HCore::SizeT u = BaseY; u < Height + BaseY; ++u) {               \
      if (ImgPtr[uA] == 0) {                                              \
        *(((volatile HCore::UInt32*)(HandoverHeader->f_GOP.f_The +        \
                                     4 *                                  \
                                         HandoverHeader->f_GOP            \
                                             .f_PixelPerLine *            \
                                         i +                              \
                                     4 * u))) |= ImgPtr[uA];              \
      } else {                                                            \
        *(((volatile HCore::UInt32*)(HandoverHeader->f_GOP.f_The +        \
                                     4 *                                  \
                                         HandoverHeader->f_GOP            \
                                             .f_PixelPerLine *            \
                                         i +                              \
                                     4 * u))) = ImgPtr[uA];               \
      }                                                                   \
                                                                          \
      ++uA;                                                               \
    }                                                                     \
  }

#endif
