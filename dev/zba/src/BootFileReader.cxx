/* -------------------------------------------

	Copyright ZKA Technologies.

	File: FileReader.cxx
	Purpose: New Boot FileReader,
	Read complete file and store it in a buffer.

------------------------------------------- */

#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>
#include <BootKit/BootKit.hxx>
#include <FirmwareKit/Handover.hxx>
#include <FirmwareKit/EFI/API.hxx>
#include <modules/FB/Text.hxx>

/// @file BootFileReader
/// @brief Bootloader File reader.
/// BUGS: 0

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///  @name BFileReader class
///  @brief Reads the file as a blob.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

/***
	@brief File Reader constructor.
*/
Boot::BFileReader::BFileReader(const CharacterTypeUTF16* path,
							   EfiHandlePtr				 ImageHandle)
{
	if (path != nullptr)
	{
		SizeT index = 0UL;
		for (; path[index] != L'\0'; ++index)
		{
			mPath[index] = path[index];
		}

		mPath[index] = 0;
	}

	/// Load protocols with their GUIDs.

	EfiGUID guidEfp = EfiGUID(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID);

	EfiSimpleFilesystemProtocol* efp = nullptr;

	EfiLoadImageProtocol* img	  = nullptr;
	EfiGUID				  guidImg = EfiGUID(EFI_LOADED_IMAGE_PROTOCOL_GUID);

	if (BS->HandleProtocol(ImageHandle, &guidImg, (void**)&img) != kEfiOk)
	{
		mWriter.Write(L"NEWOSLDR: Handle-Protocol: No-Such-Protocol").Write(L"\r");
		this->mErrorCode = kNotSupported;
	}

	if (BS->HandleProtocol(img->DeviceHandle, &guidEfp, (void**)&efp) != kEfiOk)
	{
		mWriter.Write(L"NEWOSLDR: Handle-Protocol: No-Such-Protocol").Write(L"\r");
		this->mErrorCode = kNotSupported;
		return;
	}

	/// Start doing disk I/O

	if (efp->OpenVolume(efp, &mRootFs) != kEfiOk)
	{
		mWriter.Write(L"NEWOSLDR: Fetch-Protocol: No-Such-Volume").Write(L"\r");
		this->mErrorCode = kNotSupported;
		return;
	}

	EfiFileProtocol* KernelFile = nullptr;

	if (mRootFs->Open(mRootFs, &KernelFile, mPath, kEFIFileRead, kEFIReadOnly) !=
		kEfiOk)
	{
		mWriter.Write(L"NEWOSLDR: Fetch-Protocol: No-Such-Path: ")
			.Write(mPath)
			.Write(L"\r");
		this->mErrorCode = kNotSupported;

		CGDrawString("NEWOSLDR: PLEASE RECOVER YOUR MINKRNL INSTALL.", 40, 10, RGB(0xFF, 0xFF, 0xFF));

		mRootFs->Close(mRootFs);

		return;
	}

	mSizeFile  = 0;
	mFile	   = KernelFile;
	mErrorCode = kOperationOkay;
}

Boot::BFileReader::~BFileReader()
{
	if (this->mFile)
	{
		this->mFile->Close(this->mFile);
		this->mFile = nullptr;
	}

	if (this->mRootFs)
	{
		this->mRootFs->Close(this->mRootFs);
		this->mRootFs = nullptr;
	}

	if (this->mBlob)
	{
		BS->FreePool(this->mBlob);
		this->mBlob = nullptr;
	}

	BSetMem(this->mPath, 0, kPathLen);
}

/**
	@brief Reads all of the file into a buffer.
	@param **readUntil** size of file
	@param **chunkToRead** chunk to read each time.
*/
Void Boot::BFileReader::ReadAll(SizeT readUntil, SizeT chunkToRead, UIntPtr outAddress)
{
	if (mBlob == nullptr)
	{
		EfiFileInfo newPtrInfo;
		UInt32		szInfo = 0;

		EfiGUID cFileInfoGUID = EFI_FILE_INFO_GUID;

		if (mFile->GetInfo(mFile, &cFileInfoGUID, &szInfo, &newPtrInfo) == kEfiOk)
		{
			if (newPtrInfo.FileSize < readUntil)
				readUntil = newPtrInfo.FileSize;
			else if (readUntil < 1)
				readUntil = newPtrInfo.FileSize;

			mWriter.Write(L"NEWOSLDR: SIZE: ").Write(readUntil).Write("\r");
		}

		if (!outAddress)
		{
			if (auto err = BS->AllocatePool(EfiLoaderCode, readUntil, (VoidPtr*)&mBlob) !=
						   kEfiOk)
			{
				mWriter.Write(L"*** error: ").Write(err).Write(L" ***\r");
				EFI::ThrowError(L"OutOfMemory", L"Out of memory.");
			}
		}
		else
		{
			mBlob = (VoidPtr)outAddress;
		}
	}

	mErrorCode = kNotSupported;

	UInt64 bufSize = chunkToRead;
	UInt64 szCnt   = 0UL;

	while (szCnt < readUntil)
	{
		auto res = mFile->Read(mFile, &bufSize, (VoidPtr)(&((Char*)mBlob)[szCnt]));

		szCnt += bufSize;

		if (res == kBufferTooSmall)
		{
			bufSize = chunkToRead;
		}
	}

	mSizeFile  = szCnt;
	mErrorCode = kOperationOkay;
}

/// @brief error code getter.
/// @return the error code.
Int32& Boot::BFileReader::Error()
{
	return mErrorCode;
}

/// @brief blob getter.
/// @return the blob.
VoidPtr Boot::BFileReader::Blob()
{
	return mBlob;
}

/// @breif Size getter.
/// @return the size of the file.
UInt64& Boot::BFileReader::Size()
{
	return mSizeFile;
}
