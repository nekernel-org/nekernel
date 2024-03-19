/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Stylesheet.hxx
    Purpose:

    Revision History:

    08/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

/// TODO: Stylesheets for GUI.

#include <System.Graphics/Core.hxx>
#include <System.Graphics/Dim2d.hxx>
#include <NewKit/MutableArray.hpp>

namespace HCore {
class G_API GStylesheet final {
 public:
  explicit GStylesheet() = default;
  ~GStylesheet() = default;

  HCORE_COPY_DEFAULT(GStylesheet);

  MutableArray<StringView>& Props() { return mProps; }

 private:
  MutableArray<StringView> mProps;
};

class StylesheetParser final {
 public:
  static MutableArray<GStylesheet> FromBlob(Char* Blob, SizeT BlobSz) {
    MutableArray<GStylesheet> stylesheet;

    if (!Blob || BlobSz < 1) return stylesheet;

    for (auto BlobIndex = 0UL; BlobIndex < BlobSz; ++BlobIndex) {
      
    }

    return stylesheet;
  }
};
}  // namespace HCore
