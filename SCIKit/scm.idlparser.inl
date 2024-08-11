/* -------------------------------------------

Copyright ZKA Technologies.

File: rt.internal.inl
Purpose: Base code of SCM.

------------------------------------------- */

/// @internal

#ifndef __NDK__
#define object class
#define protocol class
#define clsid(X) __attribute__((uuid(X)))

#warning ! you may be using the clang version of the newos kit, please be cautious that some thing mayn't be present. !
#endif // !__NDK__

protocol IDLParserInterface;
protocol IDLInterface;