/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 * 	
 * 	========================================================
 */
 
#pragma once
 
typedef void* ptr_type;
typedef __SIZE_TYPE__ size_type;

inline void* __cxxkit_alloca_gcc(size_type sz) { return __builtin_alloca(sz); }
