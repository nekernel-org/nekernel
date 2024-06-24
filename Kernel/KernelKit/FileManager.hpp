/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

/* -------------------------------------------

 Revision History:

	 31/01/24: Update documentation (amlel)

 ------------------------------------------- */

#pragma once

#ifdef __FSKIT_NEWFS__
#include <FSKit/NewFS.hxx>
#endif // __FSKIT_NEWFS__

#include <CompilerKit/CompilerKit.hxx>
#include <HintKit/CompilerHint.hxx>
#include <NewKit/Ref.hpp>
#include <NewKit/Stream.hpp>

/// @brief Filesystem abstraction manager.
/// Works like the VFS or IFS.

#define cRestrictR	"r"
#define cRestrictRB "rb"
#define cRestrictW	"w"
#define cRestrictRW "rw"

/// refer to first enum.
#define cFileOpsCount	 4
#define cFileMimeGeneric "application-type/*"

/** @brief invalid position. (n-pos) */
#define kNPos (SizeT)(-1);

namespace NewOS
{
	enum
	{
		cFileWriteAll	= 100,
		cFileReadAll	= 101,
		cFileReadChunk	= 102,
		cFileWriteChunk = 103,
		cFileIOCnt		= (cFileWriteChunk - cFileWriteAll) + 1,
	};

	/// @brief filesystem node generic type.
	struct PACKED FMNode final
	{
		VoidPtr _Unused;
	};

	typedef FMNode* NodePtr;

	/**
	@brief Filesystem Manager Interface class
	@brief Used to provide common I/O for a specific filesystem.
*/
	class FilesystemManagerInterface
	{
	public:
		explicit FilesystemManagerInterface() = default;
		virtual ~FilesystemManagerInterface() = default;

	public:
		NEWOS_COPY_DEFAULT(FilesystemManagerInterface);

	public:
		/// @brief Mounts a new filesystem into an active state.
		/// @param interface the filesystem interface
		/// @return
		static bool Mount(FilesystemManagerInterface* interface);

		/// @brief Unmounts the active filesystem
		/// @return
		static FilesystemManagerInterface* Unmount();

		/// @brief Getter, gets the active filesystem.
		/// @return
		static FilesystemManagerInterface* GetMounted();

	public:
		virtual NodePtr Create(_Input const char* path)			 = 0;
		virtual NodePtr CreateAlias(_Input const char* path)	 = 0;
		virtual NodePtr CreateDirectory(_Input const char* path) = 0;

	public:
		virtual bool Remove(_Input const char* path) = 0;

	public:
		virtual NodePtr Open(_Input const char* path, _Input const char* r) = 0;

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

#ifdef __FSKIT_NEWFS__
	/**
	 * @brief Based of FilesystemManagerInterface, takes care of managing NewFS
	 * disks.
	 */
	class NewFilesystemManager final : public FilesystemManagerInterface
	{
	public:
		explicit NewFilesystemManager();
		~NewFilesystemManager() override;

	public:
		NEWOS_COPY_DEFAULT(NewFilesystemManager);

	public:
		NodePtr Create(const char* path) override;
		NodePtr CreateAlias(const char* path) override;
		NodePtr CreateDirectory(const char* path) override;

	public:
		bool	Remove(const char* path) override;
		NodePtr Open(const char* path, const char* r) override;
		Void	Write(NodePtr node, VoidPtr data, Int32 flags, SizeT sz) override;
		VoidPtr Read(NodePtr node, Int32 flags, SizeT sz) override;
		bool	Seek(NodePtr node, SizeT off);
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
		void SetResourceFork(const char* forkName);
		void SetDataFork(const char* forkName);

		/// @brief Get internal parser.
		/// @return
		NewFSParser* GetParser() noexcept;

	private:
		NewFSParser* fImpl{nullptr};
	};

#endif // ifdef __FSKIT_NEWFS__

	/**
	 * Usable FileStream
	 * @tparam Encoding file encoding (char, wchar_t...)
	 * @tparam FSClass Filesystem contract who takes care of it.
	 */
	template <typename Encoding = char,
			  typename FSClass	= FilesystemManagerInterface>
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

		VoidPtr Read() noexcept
		{
			auto man = FSClass::GetMounted();

			if (man)
			{
				VoidPtr ret = man->Read(fFile, cFileReadAll, 0);
				return ret;
			}

			return nullptr;
		}

		ErrorOr<Int64> WriteAll(const char* fName, const VoidPtr data) noexcept
		{
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

		VoidPtr Read(const char* fName) noexcept
		{
			auto man = FSClass::GetMounted();

			if (man)
			{
				VoidPtr ret = man->Read(fName, fFile, cFileReadAll, 0);
				return ret;
			}

			return nullptr;
		}

		voidPtr Read(SizeT offset, SizeT sz)
		{
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
			auto man = FSClass::GetMounted();

			if (man)
			{
				man->Seek(fFile, offset);
				man->Write(fFile, data, sz, cFileReadChunk);
			}
		}

		/// @brief Leak node pointer.
		/// @return The node pointer.
		NodePtr Leak()
		{
			return fFile;
		}

	public:
		char* MIME() noexcept
		{
			return const_cast<char*>(fMime);
		}

	private:
		NodePtr		fFile;
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
		kcout << "newoskrnl: new file: " << path << ".\r";
	}

	/// @brief destructor
	template <typename Encoding, typename Class>
	FileStream<Encoding, Class>::~FileStream()
	{
		delete fFile;
	}
} // namespace NewOS

#define node_cast(PTR) reinterpret_cast<NewOS::NodePtr>(PTR)
