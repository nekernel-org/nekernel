/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */
 
#ifndef __STORAGEKIT_STORAGECORE_INL__
#define __STORAGEKIT_STORAGECORE_INL__

#include <NewKit/Defines.hpp>

// @brief Storage management unit.
// swap files, files, dirs optimization.

namespace hCore
{
	typedef Char* SKStr;
	
	//! @brief Storage context, reads and write file according to the descriptor layout.
	class StorageContext
	{
	public:
		explicit StorageContext() = default;
		~StorageContext() = default;
		
		StorageContext& operator=(const StorageContext&) = default;
		StorageContext(const StorageContext&) = default;
		
	public:
		bool Write(VoidPtr fileDescriptor, SizeT sizeFileDescriptor);
		
		struct SDescriptor
		{
			VoidPtr fFilePtr;
			SizeT fFilePtrSz;
		};
		
		SDescriptor Read(const SKStr name);
		
	};
	
#define kMaxPathSK 4096
	
	struct StorageLayout final
	{
		Char fName[kMaxPathSK];
		VoidPtr fData;	
		SizeT fDataSz;
		Int32 fType;
	};
}

#endif /* ifndef __STORAGEKIT_STORAGECORE_INL__ */
