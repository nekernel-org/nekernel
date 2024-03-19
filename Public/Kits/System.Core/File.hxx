/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef _SYSTEM_KIT_HCORE_FILE_HPP
#define _SYSTEM_KIT_HCORE_FILE_HPP

#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>

using namespace HCore;

/// @brief SOM class, translated to C++

namespace System {
class File final {
 public:
  explicit File(const char *path);
  ~File();

 public:
  HCORE_COPY_DEFAULT(File);

 public:
  voidPtr Read(SizeT off, SizeT sz);
  void Write(VoidPtr buf, SizeT off, SizeT sz);
  void Seek(SizeT off);
  voidPtr Read(SizeT sz);
  void Write(VoidPtr buf, SizeT sz);
  void Rewind();

 public:
  const char *MIME();
  void MIME(const char *mime);
};

typedef File *FilePtr;
} // namespace System

#endif  // ifndef _SYSTEM_KIT_HCORE_FILE_HPP
