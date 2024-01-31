/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _SYSTEM_KIT_HCORE_FILE_HPP
#define _SYSTEM_KIT_HCORE_FILE_HPP

#include <CompilerKit/CompilerKit.hpp>
#include <NewKit/Defines.hpp>

using namespace HCore;

class HFile final {
 public:
  explicit HFile(const char *path);
  ~HFile();

 public:
  HCORE_COPY_DEFAULT(HFile);

 public:
  voidPtr Read(SizeT off, SizeT sz);
  void Write(voidPtr buf, SizeT off, SizeT sz);
  void Seek(SizeT off);
  voidPtr Read(SizeT sz);
  void Write(voidPtr buf, SizeT sz);
  void Rewind();

 public:
  void SetMIME(const char *mime);
};

typedef HFile *HFilePtr;

#endif  // ifndef _SYSTEM_KIT_HCORE_FILE_HPP
