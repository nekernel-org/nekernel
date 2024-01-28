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

#include <CompilerKit/Compiler.hpp>
#include <NewKit/Defines.hpp>

using namespace hCore;

class MeFile final
{
  public:
    explicit MeFile(const char *path);
    ~MeFile();

  public:
    HCORE_COPY_DEFAULT(MeFile);

  public:
    void *Read(SizeT off, SizeT sz);
    void Write(void *buf, SizeT off, SizeT sz);
    void Seek(SizeT off);
    void *Read(SizeT sz);
    void Write(void *buf, SizeT sz);
    void Rewind();

  public:
    void SetMIME(const char *mime);
};

typedef MeFile *MeFilePtr;

#endif // ifndef _SYSTEM_KIT_HCORE_FILE_HPP
