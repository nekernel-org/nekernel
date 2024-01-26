/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_SYSTEM_DEF_H
#define _INC_SYSTEM_DEF_H

#ifndef Yes
#define Yes true
#endif // ifndef Yes

#ifndef No
#define No false
#endif // ifndef No

#define SizeT __SIZE_TYPE__

#define Int64 __INT64_TYPE__
#define UInt64 __UINT64_TYPE__

#define Int32 __INT32_TYPE__
#define UInt32 __UINT32_TYPE__

#define Int16 __INT16_TYPE__
#define UInt16 __UINT16_TYPE__

#define Int8 __INT8_TYPE__
#define UInt8 __UINT8_TYPE__

#define Char char
#define Boolean bool

extern "C" void __crt_must_pass(const bool eval);

#define MUST_PASS __crt_must_pass

#endif /* ifndef _INC_SYSTEM_DEF_H */
