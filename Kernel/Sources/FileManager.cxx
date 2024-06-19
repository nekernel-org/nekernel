/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <KernelKit/FileManager.hpp>
#include <NewKit/Utils.hpp>

/// BUGS: 0
//! @brief File manager for NewOS.

namespace NewOS
{
	static FilesystemManagerInterface* kMounted = nullptr;

	/// @brief FilesystemManager getter.
	/// @return The mounted filesystem.
	FilesystemManagerInterface* FilesystemManagerInterface::GetMounted()
	{
		return kMounted;
	}

	/// @brief Unmount filesystem.
	/// @return The unmounted filesystem.
	FilesystemManagerInterface* FilesystemManagerInterface::Unmount()
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
	/// @param mountPtr The filesystem to mount.
	/// @return if it succeeded true, otherwise false.
	bool FilesystemManagerInterface::Mount(FilesystemManagerInterface* mountPtr)
	{
		if (kMounted == nullptr)
		{
			kMounted = mountPtr;
			return true;
		}

		return false;
	}

#ifdef __FSKIT_NEWFS__
	/// @brief Opens a new file.
	/// @param path
	/// @param r
	/// @return
	NodePtr NewFilesystemManager::Open(const char* path, const char* r)
	{
		if (!path || *path == 0)
			return nullptr;

		if (!r || *r == 0)
			return nullptr;

		auto catalog = fImpl->GetCatalog(path);

		if (catalog->Kind != kNewFSCatalogKindFile)
		{
			fImpl->CloseCatalog(catalog);
			return nullptr;
		}

		return node_cast(catalog);
	}

	/// @brief Writes to a catalog's fork.
	/// @param node the node ptr.
	/// @param data the data.
	/// @param flags the size.
	/// @return
	Void NewFilesystemManager::Write(NodePtr node, VoidPtr data, Int32 flags, SizeT size)
	{
		if (!size ||
			size > kNewFSForkSize)
			return;

		if (!data)
			return;

		NEWOS_UNUSED(flags);

		auto dataForkName = kNewFSDataFork;

		if ((reinterpret_cast<NewCatalog*>(node))->Kind == kNewFSCatalogKindFile)
			fImpl->WriteCatalog(reinterpret_cast<NewCatalog*>(node), data, size,
								dataForkName);
	}

	/// @brief Read from filesystem fork.
	/// @param node the catalog node.
	/// @param flags the flags with it.
	/// @param sz the size to read.
	/// @return
	VoidPtr NewFilesystemManager::Read(NodePtr node, Int32 flags, SizeT sz)
	{
		if (sz > kNewFSForkSize)
			return nullptr;

		if (!sz)
			return nullptr;

		NEWOS_UNUSED(flags);

		auto dataForkName = kNewFSDataFork;

		if ((reinterpret_cast<NewCatalog*>(node))->Kind == kNewFSCatalogKindFile)
			return fImpl->ReadCatalog(reinterpret_cast<NewCatalog*>(node), sz,
									  dataForkName);

		return nullptr;
	}

	/// @brief Seek from Catalog.
	/// @param node
	/// @param off
	/// @retval true always returns false, this is unimplemented.
	/// @retval false always returns this, it is unimplemented.

	bool NewFilesystemManager::Seek(NodePtr node, SizeT off)
	{
		if (!node || off == 0)
			return false;

		return fImpl->Seek(reinterpret_cast<NewCatalog*>(node), off);
	}

	/// @brief Tell where the catalog is.
	/// @param node
	/// @retval true always returns false, this is unimplemented.
	/// @retval false always returns this, it is unimplemented.

	SizeT NewFilesystemManager::Tell(NodePtr node)
	{
		if (!node)
			return kNPos;

		return fImpl->Tell(reinterpret_cast<NewCatalog*>(node));
	}

	/// @brief Rewinds the catalog.
	/// @param node
	/// @retval true always returns false, this is unimplemented.
	/// @retval false always returns this, it is unimplemented.

	bool NewFilesystemManager::Rewind(NodePtr node)
	{
		if (!node)
			return false;

		return this->Seek(node, 0);
	}

	/// @brief Returns the filesystem parser.
	/// @return the Filesystem parser class.
	NewFSParser* NewFilesystemManager::GetParser() noexcept
	{
		return fImpl;
	}
#endif // __FSKIT_NEWFS__
} // namespace NewOS
