/* -------------------------------------------

	Copyright ZKA Technologies.

	File: FileMgr.hxx
	Purpose: Kernel file manager.

------------------------------------------- */

/* -------------------------------------------

 Revision History:

	 31/01/24: Update documentation (amlel)
	 05/07/24: NeFS support, and fork support, updated constants and specs
		as well.

 ------------------------------------------- */

#pragma once

#ifdef __FSKIT_USE_NEFS__
#include <FSKit/NeFS.hxx>
#endif // __FSKIT_USE_NEFS__

#include <CompilerKit/CompilerKit.hxx>
#include <HintKit/CompilerHint.hxx>
#include <KernelKit/LPC.hxx>
#include <KernelKit/DebugOutput.hxx>
#include <NewKit/Stream.hxx>
#include <NewKit/ErrorOr.hxx>
#include <KernelKit/Heap.hxx>
#include <NewKit/Ref.hxx>

/// @brief Filesystem manager, abstraction over mounted filesystem.
/// Works like the VFS or IFS.

#define cRestrictR	 "r"
#define cRestrictRB	 "rb"
#define cRestrictW	 "w"
#define cRestrictWB	 "rw"
#define cRestrictRWB "rwb"

#define cRestrictMax 5

#define node_cast(PTR) reinterpret_cast<Kernel::NodePtr>(PTR)

/**
	@note Refer to first enum.
*/
#define cFileOpsCount	 4
#define cFileMimeGeneric "n-application-kind/all"

/** @brief invalid position. (n-pos) */
#define kNPos (SizeT)(-1);

namespace Kernel
{
	enum
	{
		cFileWriteAll	= 100,
		cFileReadAll	= 101,
		cFileReadChunk	= 102,
		cFileWriteChunk = 103,
		cFileIOCnt		= (cFileWriteChunk - cFileWriteAll) + 1,
		// file flags
		cFileFlagRsrc = 104,
		cFileFlagData = 105,
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

#ifdef __FSKIT_USE_NEFS__
	/**
	 * @brief Based of IFilesystemMgr, takes care of managing NeFS
	 * disks.
	 */
	class NewFilesystemMgr final : public IFilesystemMgr
	{
	public:
		explicit NewFilesystemMgr();
		~NewFilesystemMgr() override;

	public:
		ZKA_COPY_DEFAULT(NewFilesystemMgr);

	public:
		NodePtr Create(const Char* path) override;
		NodePtr CreateAlias(const Char* path) override;
		NodePtr CreateDirectory(const Char* path) override;
		NodePtr CreateSwapFile(const Char* path) override;

	public:
		bool	Remove(const Char* path) override;
		NodePtr Open(const Char* path, const Char* r) override;
		Void	Write(NodePtr node, VoidPtr data, Int32 flags, SizeT sz) override;
		VoidPtr Read(NodePtr node, Int32 flags, SizeT sz) override;
		bool	Seek(NodePtr node, SizeT off) override;
		SizeT	Tell(NodePtr node) override;
		bool	Rewind(NodePtr node) override;

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
		NeFSParser* GetParser() noexcept;

	private:
		NeFSParser* fImpl{nullptr};
	};

#endif // ifdef __FSKIT_USE_NEFS__

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
			if (this->fFileRestrict != eRestrictReadWrite &&
				this->fFileRestrict != eRestrictReadWriteBinary &&
				this->fFileRestrict != eRestrictWrite &&
				this->fFileRestrict != eRestrictWriteBinary)
				return ErrorOr<Int64>(kErrorInvalidData);

			if (data == nullptr)
				return ErrorOr<Int64>(kErrorInvalidData);

			auto man = FSClass::GetMounted();

			if (man)
			{
				man->Write(fFile, data, cFileWriteAll);
				return ErrorOr<Int64>(0);
			}

			return ErrorOr<Int64>(kErrorInvalidData);
		}

		VoidPtr ReadAll() noexcept
		{
			if (this->fFileRestrict != eRestrictReadWrite &&
				this->fFileRestrict != eRestrictReadWriteBinary &&
				this->fFileRestrict != eRestrictRead &&
				this->fFileRestrict != eRestrictReadBinary)
				return nullptr;

			auto man = FSClass::GetMounted();

			if (man)
			{
				VoidPtr ret = man->Read(fFile, cFileReadAll, 0);
				return ret;
			}

			return nullptr;
		}

		ErrorOr<Int64> WriteAll(const Char* fName, const VoidPtr data) noexcept
		{
			if (this->fFileRestrict != eRestrictReadWrite &&
				this->fFileRestrict != eRestrictReadWriteBinary &&
				this->fFileRestrict != eRestrictWrite &&
				this->fFileRestrict != eRestrictWriteBinary)
				return ErrorOr<Int64>(kErrorInvalidData);

			if (data == nullptr)
				return ErrorOr<Int64>(kErrorInvalidData);

			auto man = FSClass::GetMounted();

			if (man)
			{
				man->Write(fName, fFile, data, cFileWriteAll);
				return ErrorOr<Int64>(0);
			}

			return ErrorOr<Int64>(kErrorInvalidData);
		}

		VoidPtr Read(const Char* fName) noexcept
		{
			if (this->fFileRestrict != eRestrictReadWrite &&
				this->fFileRestrict != eRestrictReadWriteBinary &&
				this->fFileRestrict != eRestrictRead &&
				this->fFileRestrict != eRestrictReadBinary)
				return nullptr;

			auto man = FSClass::GetMounted();

			if (man)
			{
				VoidPtr ret = man->Read(fName, fFile, cFileReadAll, 0);
				return ret;
			}

			return nullptr;
		}

		VoidPtr Read(SizeT offset, SizeT sz)
		{
			if (this->fFileRestrict != eRestrictReadWrite &&
				this->fFileRestrict != eRestrictReadWriteBinary &&
				this->fFileRestrict != eRestrictRead &&
				this->fFileRestrict != eRestrictReadBinary)
				return nullptr;

			auto man = FSClass::GetMounted();

			if (man)
			{
				man->Seek(fFile, offset);
				auto ret = man->Read(fFile, cFileReadChunk, sz);

				return ret;
			}

			return nullptr;
		}

		Void Write(SizeT offset, voidPtr data, SizeT sz)
		{
			if (this->fFileRestrict != eRestrictReadWrite &&
				this->fFileRestrict != eRestrictReadWriteBinary &&
				this->fFileRestrict != eRestrictWrite &&
				this->fFileRestrict != eRestrictWriteBinary)
				return;

			auto man = FSClass::GetMounted();

			if (man)
			{
				man->Seek(fFile, offset);
				man->Write(fFile, data, sz, cFileReadChunk);
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
			return const_cast<char*>(fMime);
		}

		enum
		{
			eRestrictRead,
			eRestrictReadBinary,
			eRestrictWrite,
			eRestrictWriteBinary,
			eRestrictReadWrite,
			eRestrictReadWriteBinary,
		};

	private:
		NodePtr		fFile{nullptr};
		Int32		fFileRestrict{};
		const Char* fMime{cFileMimeGeneric};
	};

	using FileStreamUTF8  = FileStream<Char>;
	using FileStreamUTF16 = FileStream<WideChar>;

	typedef UInt64 CursorType;

	/// @brief constructor
	template <typename Encoding, typename Class>
	FileStream<Encoding, Class>::FileStream(const Encoding* path,
											const Encoding* restrict_type)
		: fFile(Class::GetMounted()->Open(path, restrict_type))
	{
		static const auto cLength = 255;

		/// @brief restrict information about the file descriptor.
		struct RESTRICT_MAP final
		{
			Char  fRestrict[cLength];
			Int32 fMappedTo;
		};

		const SizeT		   cRestrictCount  = cRestrictMax;
		const RESTRICT_MAP cRestrictList[] = {
			{
				.fRestrict = cRestrictR,
				.fMappedTo = eRestrictRead,
			},
			{
				.fRestrict = cRestrictRB,
				.fMappedTo = eRestrictReadBinary,
			},
			{
				.fRestrict = cRestrictRWB,
				.fMappedTo = eRestrictReadWriteBinary,
			},
			{
				.fRestrict = cRestrictW,
				.fMappedTo = eRestrictWrite,
			},
			{
				.fRestrict = cRestrictWB,
				.fMappedTo = eRestrictReadWrite,
			}};

		for (SizeT index = 0; index < cRestrictCount; ++index)
		{
			if (rt_string_cmp(restrict_type, cRestrictList[index].fRestrict,
							  rt_string_len(cRestrictList[index].fRestrict)) == 0)
			{
				fFileRestrict = cRestrictList[index].fMappedTo;
				break;
			}
		}

		kcout << "new file: " << path << ".\r";
	}

	/// @brief destructor
	template <typename Encoding, typename Class>
	FileStream<Encoding, Class>::~FileStream()
	{
		mm_delete_ke_heap(fFile);
	}
} // namespace Kernel
