/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_CL_HPP
#define _INC_CL_HPP

#include <NewKit/Defines.hpp>

#define HCORE_COPY_DELETE(KLASS)                                                                                         \
    KLASS &operator=(const KLASS &) = delete;                                                                          \
    KLASS(const KLASS &) = delete;


#define HCORE_COPY_DEFAULT(KLASS)                                                                                        \
    KLASS &operator=(const KLASS &) = default;                                                                         \
    KLASS(const KLASS &) = default;


#define HCORE_MOVE_DELETE(KLASS)                                                                                         \
    KLASS &operator=(KLASS &&) = delete;                                                                               \
    KLASS(KLASS &&) = delete;


#define HCORE_MOVE_DEFAULT(KLASS)                                                                                        \
    KLASS &operator=(KLASS &&) = default;                                                                              \
    KLASS(KLASS &&) = default;



#endif /* ifndef _INC_CL_HPP */
