/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/FileMgr.hxx>
#include <NewKit/Utils.hxx>

/// BUGS: 0
//! @brief File manager for Kernel.

namespace Kernel
{
	STATIC FilesystemMgrInterface* kMounted = nullptr;

	/// @brief FilesystemMgr getter.
	/// @return The mounted filesystem.
	_Output FilesystemMgrInterface* FilesystemMgrInterface::GetMounted()
	{
		return kMounted;
	}

	/// @brief Unmount filesystem.
	/// @return The unmounted filesystem.
	_Output FilesystemMgrInterface* FilesystemMgrInterface::Unmount()
	{
		if (kMounted)
		{
			auto mount = kMounted;
			kMounted   = nullptr;

			return mount;
		}

		return nullptr;
	}

	/// @brief Mount filesystem.
	/// @param mount_ptr The filesystem to mount.
	/// @return if it succeeded true, otherwise false.
	bool FilesystemMgrInterface::Mount(_Input FilesystemMgrInterface* mount_ptr)
	{
		if (mount_ptr != nullptr)
		{
			kMounted = mount_ptr;
			return true;
		}

		return false;
	}

#ifdef __FSKIT_USE_NEWFS__
	/// @brief Opens a new file.
	/// @param path
	/// @param r
	/// @return
	_Output NodePtr NewFilesystemMgr::Open(_Input const Char* path, _Input const Char* r)
	{
		if (!path || *path == 0)
			return nullptr;

		if (!r || *r == 0)
			return nullptr;

		auto catalog = fImpl->GetCatalog(path);

		return node_cast(catalog);
	}

	/// @brief Writes to a catalog's fork.
	/// @param node the node ptr.
	/// @param data the data.
	/// @param flags the size.
	/// @return
	Void NewFilesystemMgr::Write(_Input NodePtr node, _Input VoidPtr data, _Input Int32 flags, _Input SizeT size)
	{
		if (!node)
			return;
		if (!size)
			return;

		constexpr auto cDataForkName = kNewFSDataFork;
		this->Write(cDataForkName, node, data, flags, size);
	}

	/// @brief Read from filesystem fork.
	/// @param node the catalog node.
	/// @param flags the flags with it.
	/// @param sz the size to read.
	/// @return
	_Output VoidPtr NewFilesystemMgr::Read(_Input NodePtr node, _Input Int32 flags, _Input SizeT size)
	{
		if (!node)
			return nullptr;
		if (!size)
			return nullptr;

		constexpr auto cDataForkName = kNewFSDataFork;
		return this->Read(cDataForkName, node, flags, size);
	}

	Void NewFilesystemMgr::Write(_Input const Char* name,
									 _Input NodePtr		node,
									 _Input VoidPtr		data,
									 _Input Int32		flags,
									 _Input SizeT		size)
	{
		if (!size ||
			size > kNewFSForkSize)
			return;

		if (!data)
			return;

		ZKA_UNUSED(flags);

		if ((reinterpret_cast<NFS_CATALOG_STRUCT*>(node))->Kind == kNewFSCatalogKindFile)
			fImpl->WriteCatalog(reinterpret_cast<NFS_CATALOG_STRUCT*>(node), (flags & cFileFlagRsrc ? true : false), data, size,
								name);
	}

	_Output VoidPtr NewFilesystemMgr::Read(_Input const Char* name,
											   _Input NodePtr	  node,
											   _Input Int32		  flags,
											   _Input SizeT		  sz)
	{
		if (sz > kNewFSForkSize)
			return nullptr;

		if (!sz)
			return nullptr;

		ZKA_UNUSED(flags);

		if ((reinterpret_cast<NFS_CATALOG_STRUCT*>(node))->Kind == kNewFSCatalogKindFile)
			return fImpl->ReadCatalog(reinterpret_cast<NFS_CATALOG_STRUCT*>(node), (flags & cFileFlagRsrc ? true : false), sz,
									  name);

		return nullptr;
	}

	/// @brief Seek from Catalog.
	/// @param node
	/// @param off
	/// @retval true always returns false, this is unimplemented.
	/// @retval false always returns this, it is unimplemented.

	_Output Bool NewFilesystemMgr::Seek(NodePtr node, SizeT off)
	{
		if (!node || off == 0)
			return false;

		return fImpl->Seek(reinterpret_cast<NFS_CATALOG_STRUCT*>(node), off);
	}

	/// @brief Tell where the catalog is.
	/// @param node
	/// @retval true always returns false, this is unimplemented.
	/// @retval false always returns this, it is unimplemented.

	_Output SizeT NewFilesystemMgr::Tell(NodePtr node)
	{
		if (!node)
			return kNPos;

		return fImpl->Tell(reinterpret_cast<NFS_CATALOG_STRUCT*>(node));
	}

	/// @brief Rewinds the catalog.
	/// @param node
	/// @retval true always returns false, this is unimplemented.
	/// @retval false always returns this, it is unimplemented.

	_Output Bool NewFilesystemMgr::Rewind(NodePtr node)
	{
		if (!node)
			return false;

		return this->Seek(node, 0);
	}

	/// @brief Returns the filesystem parser.
	/// @return the Filesystem parser class.
	_Output NewFSParser* NewFilesystemMgr::GetParser() noexcept
	{
		return fImpl;
	}
#endif // __FSKIT_USE_NEWFS__
} // namespace Kernel
