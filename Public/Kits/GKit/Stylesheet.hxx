/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Stylesheet.hpp
    Purpose:

    Revision History:

    08/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

/// TODO: Stylesheets for GUI.

#include <GKit/Core.hxx>
#include <GKit/Dim2d.hxx>
#include <NewKit/MutableArray.hpp>

namespace HCore {
class G_API Stylesheet final {
 public:
  explicit Stylesheet() = default;
  ~Stylesheet() = default;

  HCORE_COPY_DEFAULT(Stylesheet);

  MutableArray<StringView>& Props() { return mProps; }

 private:
  MutableArray<StringView> mProps;
};

class StylesheetParser final {
 public:
  static MutableArray<Stylesheet> FromBlob(WideChar* Blob, SizeT BlobSz) {
    MutableArray<Stylesheet> stylesheet;

    if (!Blob || BlobSz < 1) return stylesheet;

    for (auto BlobIndex = 0UL; BlobIndex < BlobSz; ++BlobIndex) {
    }

    return stylesheet;
  }
};
}  // namespace HCore
