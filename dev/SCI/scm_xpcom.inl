/* -------------------------------------------

Copyright ZKA Technologies.

File: rt.internal.inl
Purpose: Base code of SCM.

------------------------------------------- */

/// @internal

#ifndef __NDK__
#define object class
#define protocol class
#define interface private
#define method
#define clsid(X)

#warning ! you may be using the clang version of the newos kit, please be cautious that some thing mayn't be present. !
#endif // !__NDK__

protocol IPXCOMSocket;

clsid("0943A614-0201-4107-8F8D-E909DF7F53C9")
protocol IPXCOMSocket
{
interface:
    method SInt32 SendMessage(const Char* bytes, SizeT bytes_size);
    method SInt32 RecvMessage(_Output const Char** bytes_in, _Input SizeT bytes_size);
};
