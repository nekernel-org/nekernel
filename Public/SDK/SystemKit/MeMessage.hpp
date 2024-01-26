/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_MESSAGE_HPP
#define _INC_MESSAGE_HPP

#include <CompilerKit/Compiler.hpp>
#include <SystemKit/Defines.hpp>

// bugs: 0 (see .cpp)

class MeMessage final
{
public:
    MeMessage() = default;
    ~MeMessage() = default;

public:
    HCORE_COPY_DEFAULT(MeMessage);

public:
    void SendInt8(Int8 num);
    void SendUInt8(UInt8 num);

public:
    void SendInt16(Int16 num);
    void SendUInt16(UInt16 num);

public:
    void SendInt32(Int32 num);
    void SendUInt32(UInt32 num);

public:
    void SendInt64(Int64 num);
    void SendUInt64(UInt64 num);

public:
    void SendAnything(void* ptr, const char* name);
    void SendClass(void* class_ptr);
    void SendZip(void* zip_ptr);

public:
    void* RecvAnything(const char* the_name);
    void* RecvClass(const char* name, Int32 pos);
    void* RecvZip(const char* name);

};

#endif // _INC_MESSAGE_HPP
