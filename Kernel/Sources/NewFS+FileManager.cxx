/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <KernelKit/FileManager.hxx>
#include <KernelKit/Heap.hxx>

#ifdef __FSKIT_USE_NEWFS__

/// @brief NewFS File manager.
/// BUGS: 0

namespace Kernel
{
	/// @brief C++ constructor
	NewFilesystemManager::NewFilesystemManager()
	{
		MUST_PASS(Detail::fs_init_newfs());
		fImpl = new NewFSParser();

		kcout << "newoskrnl: We are done here... (NewFilesystemManager).\r";
	}

	NewFilesystemManager::~NewFilesystemManager()
	{
		kcout << "newoskrnl: Destroying it...\r";

		if (fImpl)
		{
			delete fImpl;
		}
	}

	/// @brief Removes a node from the filesystem.
	/// @param fileName The filename
	/// @return If it was deleted or not.
	bool NewFilesystemManager::Remove(const char* fileName)
	{
		if (fileName == nullptr || *fileName == 0)
			return false;

		return fImpl->RemoveCatalog(fileName);
	}

	/// @brief Creates a node with the specified.
	/// @param path The filename path.
	/// @return The Node pointer.
	NodePtr NewFilesystemManager::Create(const char* path)
	{
		return node_cast(fImpl->CreateCatalog(path, 0, kNewFSCatalogKindFile));
	}

	/// @brief Creates a node with is a directory.
	/// @param path The filename path.
	/// @return The Node pointer.
	NodePtr NewFilesystemManager::CreateDirectory(const char* path)
	{
		return node_cast(fImpl->CreateCatalog(path, 0, kNewFSCatalogKindDir));
	}

	/// @brief Creates a node with is a alias.
	/// @param path The filename path.
	/// @return The Node pointer.
	NodePtr NewFilesystemManager::CreateAlias(const char* path)
	{
		return node_cast(fImpl->CreateCatalog(path, 0, kNewFSCatalogKindAlias));
	}

	/// @brief Gets the root directory.
	/// @return
	const char* NewFilesystemHelper::Root()
	{
		return kNewFSRoot;
	}

	/// @brief Gets the up-dir directory.
	/// @return
	const char* NewFilesystemHelper::UpDir()
	{
		return kNewFSUpDir;
	}

	/// @brief Gets the separator character.
	/// @return
	const char NewFilesystemHelper::Separator()
	{
		return kNewFSSeparator;
	}

	/// @brief Gets the metafile character.
	/// @return 
	const char NewFilesystemHelper::MetaFile()
	{
		return kNewFSMetaFilePrefix;
	}
} // namespace Kernel

#endif // ifdef __FSKIT_USE_NEWFS__
