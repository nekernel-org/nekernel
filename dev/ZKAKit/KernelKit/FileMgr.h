/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

	File: FileMgr.h
	Purpose: Kernel file manager.

------------------------------------------- */

/* -------------------------------------------

 Revision History:

	 31/01/24: Update documentation (amlel)
	 05/07/24: NeFS support, and fork support, updated constants and specs
		as well.

 ------------------------------------------- */

#ifndef INC_FILEMGR_H
#define INC_FILEMGR_H

#ifdef __FSKIT_INCLUDES_NEFS__
#include <FSKit/NeFS.h>
#endif // __FSKIT_INCLUDES_NEFS__

#ifdef __FSKIT_INCLUDES_HPFS__
#include <FSKit/HPFS.h>
#endif // __FSKIT_INCLUDES_HPFS__

#include <CompilerKit/CompilerKit.h>
#include <HintKit/CompilerHint.h>
#include <KernelKit/LPC.h>
#include <KernelKit/DebugOutput.h>
#include <NewKit/Stream.h>
#include <NewKit/ErrorOr.h>
#include <KernelKit/Heap.h>
#include <NewKit/Ref.h>

/// @brief Filesystem manager, abstraction over mounted filesystem.
/// Works like the VFS or IFS.

#define kRestrictR	 "r"
#define kRestrictRB	 "rb"
#define kRestrictW	 "w"
#define kRestrictWB	 "rw"
#define kRestrictRWB "rwb"

#define kRestrictMax (5U)

#define node_cast(PTR) reinterpret_cast<Kernel::NodePtr>(PTR)

/**
	@note Refer to first enum.
*/
#define kFileOpsCount	 (4U)
#define kFileMimeGeneric "n-application-kind/all"

/** @brief invalid position. (n-pos) */
#define kNPos (SizeT)(-1);

namespace Kernel
{
	enum
	{
		kFileWriteAll	= 100,
		kFileReadAll	= 101,
		kFileReadChunk	= 102,
		kFileWriteChunk = 103,
		kFileIOCnt		= (kFileWriteChunk - kFileWriteAll) + 1,
		// file flags
		kFileFlagRsrc = 104,
		kFileFlagData = 105,
	};

	typedef VoidPtr NodePtr;

	/**
	@brief Filesystem Mgr Interface class
	@brief Used to provide common I/O for a specific filesystem.
*/
	class IFilesystemMgr
	{
	public:
		explicit IFilesystemMgr() = default;
		virtual ~IFilesystemMgr() = default;

	public:
		ZKA_COPY_DEFAULT(IFilesystemMgr);

	public:
		/// @brief Mounts a new filesystem into an active state.
		/// @param interface the filesystem interface
		/// @return
		static bool Mount(IFilesystemMgr* interface);

		/// @brief Unmounts the active filesystem
		/// @return
		static IFilesystemMgr* Unmount();

		/// @brief Getter, gets the active filesystem.
		/// @return
		static IFilesystemMgr* GetMounted();

	public:
		virtual NodePtr Create(_Input const Char* path)			 = 0;
		virtual NodePtr CreateAlias(_Input const Char* path)	 = 0;
		virtual NodePtr CreateDirectory(_Input const Char* path) = 0;
		virtual NodePtr CreateSwapFile(const Char* path)		 = 0;

	public:
		virtual bool Remove(_Input const Char* path) = 0;

	public:
		virtual NodePtr Open(_Input const Char* path, _Input const Char* r) = 0;

	public:
		virtual Void Write(_Input NodePtr node, _Input VoidPtr data, _Input Int32 flags, _Input SizeT size) = 0;

		virtual _Output VoidPtr Read(_Input NodePtr node,
									 _Input Int32	flags,
									 _Input SizeT	sz) = 0;

		virtual Void Write(_Input const Char* name,
						   _Input NodePtr	  node,
						   _Input VoidPtr	  data,
						   _Input Int32		  flags,
						   _Input SizeT		  size) = 0;

		virtual _Output VoidPtr Read(_Input const Char* name,
									 _Input NodePtr		node,
									 _Input Int32		flags,
									 _Input SizeT		sz) = 0;

	public:
		virtual bool Seek(_Input NodePtr node, _Input SizeT off) = 0;

	public:
		virtual SizeT Tell(_Input NodePtr node)	  = 0;
		virtual bool  Rewind(_Input NodePtr node) = 0;
	};

#ifdef __FSKIT_INCLUDES_NEFS__
	/**
	 * @brief Based of IFilesystemMgr, takes care of managing NeFS
	 * disks.
	 */
	class NeFileSystemMgr final : public IFilesystemMgr
	{
	public:
		explicit NeFileSystemMgr();
		~NeFileSystemMgr() override;

	public:
		ZKA_COPY_DEFAULT(NeFileSystemMgr);

	public:
		NodePtr Create(const Char* path) override;
		NodePtr CreateAlias(const Char* path) override;
		NodePtr CreateDirectory(const Char* path) override;
		NodePtr CreateSwapFile(const Char* path) override;

	public:
		bool	Remove(_Input const Char* path) override;
		NodePtr Open(_Input const Char* path, _Input const Char* r) override;
		Void	Write(_Input NodePtr node, _Input VoidPtr data, _Input Int32 flags, _Input SizeT sz) override;
		VoidPtr Read(_Input NodePtr node, _Input Int32 flags, _Input SizeT sz) override;
		bool	Seek(_Input NodePtr node, _Input SizeT off) override;
		SizeT	Tell(_Input NodePtr node) override;
		bool	Rewind(_Input NodePtr node) override;

		Void Write(_Input const Char* name,
				   _Input NodePtr	  node,
				   _Input VoidPtr	  data,
				   _Input Int32		  flags,
				   _Input SizeT		  size) override;

		_Output VoidPtr Read(_Input const Char* name,
							 _Input NodePtr		node,
							 _Input Int32		flags,
							 _Input SizeT		sz) override;

	public:
		/// @brief Get NeFS parser class.
		/// @return The filesystem parser class.
		NeFileSystemParser* GetParser() noexcept;

	private:
		NeFileSystemParser* fImpl{nullptr};
	};

#endif // ifdef __FSKIT_INCLUDES_NEFS__

	/**
	 * Usable FileStream
	 * @tparam Encoding file encoding (char, wchar_t...)
	 * @tparam FSClass Filesystem contract who takes care of it.
	 */
	template <typename Encoding = Char,
			  typename FSClass	= IFilesystemMgr>
	class FileStream final
	{
	public:
		explicit FileStream(const Encoding* path, const Encoding* restrict_type);
		~FileStream();

	public:
		FileStream& operator=(const FileStream&);
		FileStream(const FileStream&);

	public:
		ErrorOr<Int64> WriteAll(const VoidPtr data) noexcept
		{
			if (this->fFileRestrict != kFileMgrRestrictReadWrite &&
				this->fFileRestrict != kFileMgrRestrictReadWriteBinary &&
				this->fFileRestrict != kFileMgrRestrictWrite &&
				this->fFileRestrict != kFileMgrRestrictWriteBinary)
				return ErrorOr<Int64>(kErrorInvalidData);

			if (data == nullptr)
				return ErrorOr<Int64>(kErrorInvalidData);

			auto man = FSClass::GetMounted();

			if (man)
			{
				man->Write(fFile, data, kFileWriteAll);
				return ErrorOr<Int64>(0);
			}

			return ErrorOr<Int64>(kErrorInvalidData);
		}

		VoidPtr ReadAll() noexcept
		{
			if (this->fFileRestrict != kFileMgrRestrictReadWrite &&
				this->fFileRestrict != kFileMgrRestrictReadWriteBinary &&
				this->fFileRestrict != kFileMgrRestrictRead &&
				this->fFileRestrict != kFileMgrRestrictReadBinary)
				return nullptr;

			auto man = FSClass::GetMounted();

			if (man)
			{
				VoidPtr ret = man->Read(fFile, kFileReadAll, 0);
				return ret;
			}

			return nullptr;
		}

		ErrorOr<Int64> WriteAll(const Char* fName, const VoidPtr data) noexcept
		{
			if (this->fFileRestrict != kFileMgrRestrictReadWrite &&
				this->fFileRestrict != kFileMgrRestrictReadWriteBinary &&
				this->fFileRestrict != kFileMgrRestrictWrite &&
				this->fFileRestrict != kFileMgrRestrictWriteBinary)
				return ErrorOr<Int64>(kErrorInvalidData);

			if (data == nullptr)
				return ErrorOr<Int64>(kErrorInvalidData);

			auto man = FSClass::GetMounted();

			if (man)
			{
				man->Write(fName, fFile, data, kFileWriteAll);
				return ErrorOr<Int64>(0);
			}

			return ErrorOr<Int64>(kErrorInvalidData);
		}

		VoidPtr Read(const Char* fName) noexcept
		{
			if (this->fFileRestrict != kFileMgrRestrictReadWrite &&
				this->fFileRestrict != kFileMgrRestrictReadWriteBinary &&
				this->fFileRestrict != kFileMgrRestrictRead &&
				this->fFileRestrict != kFileMgrRestrictReadBinary)
				return nullptr;

			auto man = FSClass::GetMounted();

			if (man)
			{
				VoidPtr ret = man->Read(fName, fFile, kFileReadAll, 0);
				return ret;
			}

			return nullptr;
		}

		VoidPtr Read(SizeT offset, SizeT sz)
		{
			if (this->fFileRestrict != kFileMgrRestrictReadWrite &&
				this->fFileRestrict != kFileMgrRestrictReadWriteBinary &&
				this->fFileRestrict != kFileMgrRestrictRead &&
				this->fFileRestrict != kFileMgrRestrictReadBinary)
				return nullptr;

			auto man = FSClass::GetMounted();

			if (man)
			{
				man->Seek(fFile, offset);
				auto ret = man->Read(fFile, kFileReadChunk, sz);

				return ret;
			}

			return nullptr;
		}

		Void Write(SizeT offset, voidPtr data, SizeT sz)
		{
			if (this->fFileRestrict != kFileMgrRestrictReadWrite &&
				this->fFileRestrict != kFileMgrRestrictReadWriteBinary &&
				this->fFileRestrict != kFileMgrRestrictWrite &&
				this->fFileRestrict != kFileMgrRestrictWriteBinary)
				return;

			auto man = FSClass::GetMounted();

			if (man)
			{
				man->Seek(fFile, offset);
				man->Write(fFile, data, sz, kFileReadChunk);
			}
		}

	public:
		/// @brief Leak node pointer.
		/// @return The node pointer.
		NodePtr Leak()
		{
			return fFile;
		}

		/// @brief Leak MIME.
		/// @return The MIME.
		Char* MIME() noexcept
		{
			return const_cast<Char*>(fMime);
		}

		enum
		{
			kFileMgrRestrictRead,
			kFileMgrRestrictReadBinary,
			kFileMgrRestrictWrite,
			kFileMgrRestrictWriteBinary,
			kFileMgrRestrictReadWrite,
			kFileMgrRestrictReadWriteBinary,
		};

	private:
		NodePtr		fFile{nullptr};
		Int32		fFileRestrict{kFileMgrRestrictReadBinary};
		const Char* fMime{kFileMimeGeneric};
	};

	using FileStreamUTF8  = FileStream<Char>;
	using FileStreamUTF16 = FileStream<WideChar>;

	typedef UInt64 CursorType;

	inline static const auto kRestrictStrLen = 8U;

	/// @brief restrict information about the file descriptor.
	struct FileRestrictKind final
	{
		Char  fRestrict[kRestrictStrLen];
		Int32 fMappedTo;
	};

	/// @brief constructor
	template <typename Encoding, typename Class>
	inline FileStream<Encoding, Class>::FileStream(const Encoding* path,
												   const Encoding* restrict_type)
		: fFile(Class::GetMounted()->Open(path, restrict_type))
	{
		const SizeT			   kRestrictCount  = kRestrictMax;
		const FileRestrictKind kRestrictList[] = {
			{
				.fRestrict = kRestrictR,
				.fMappedTo = kFileMgrRestrictRead,
			},
			{
				.fRestrict = kRestrictRB,
				.fMappedTo = kFileMgrRestrictReadBinary,
			},
			{
				.fRestrict = kRestrictRWB,
				.fMappedTo = kFileMgrRestrictReadWriteBinary,
			},
			{
				.fRestrict = kRestrictW,
				.fMappedTo = kFileMgrRestrictWrite,
			},
			{
				.fRestrict = kRestrictWB,
				.fMappedTo = kFileMgrRestrictReadWrite,
			}};

		for (SizeT index = 0; index < kRestrictCount; ++index)
		{
			if (rt_string_cmp(restrict_type, kRestrictList[index].fRestrict,
							  rt_string_len(kRestrictList[index].fRestrict)) == 0)
			{
				fFileRestrict = kRestrictList[index].fMappedTo;
				break;
			}
		}

		kcout << "new file: " << path << ".\r";
	}

	/// @brief destructor of the file stream.
	template <typename Encoding, typename Class>
	inline FileStream<Encoding, Class>::~FileStream()
	{
		mm_delete_heap(fFile);
	}
} // namespace Kernel

#endif // ifndef INC_FILEMGR_H
