/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/FileMgr.hxx>
#include <KernelKit/Heap.hxx>

#ifdef __FSKIT_USE_NEFS__

/// @brief NeFS File manager.
/// BUGS: 0

namespace Kernel
{
	/// @brief C++ constructor
	NewFilesystemMgr::NewFilesystemMgr()
	{
		MUST_PASS(Detail::fs_init_newfs());
		fImpl = mm_new_class<NeFSParser>();
		MUST_PASS(fImpl);

		kcout << "We are done here... (NewFilesystemMgr).\r";
	}

	NewFilesystemMgr::~NewFilesystemMgr()
	{
		kcout << "Destroying it...\r";

		if (fImpl)
		{
			delete fImpl;
		}
	}

	/// @brief Removes a node from the filesystem.
	/// @param fileName The filename
	/// @return If it was deleted or not.
	bool NewFilesystemMgr::Remove(const Char* fileName)
	{
		if (fileName == nullptr || *fileName == 0)
			return false;

		return fImpl->RemoveCatalog(fileName);
	}

	/// @brief Creates a node with the specified.
	/// @param path The filename path.
	/// @return The Node pointer.
	NodePtr NewFilesystemMgr::Create(const Char* path)
	{
		return node_cast(fImpl->CreateCatalog(path));
	}

	/// @brief Creates a node with is a directory.
	/// @param path The filename path.
	/// @return The Node pointer.
	NodePtr NewFilesystemMgr::CreateDirectory(const Char* path)
	{
		return node_cast(fImpl->CreateCatalog(path, 0, kNeFSCatalogKindDir));
	}

	/// @brief Creates a node with is a alias.
	/// @param path The filename path.
	/// @return The Node pointer.
	NodePtr NewFilesystemMgr::CreateAlias(const Char* path)
	{
		return node_cast(fImpl->CreateCatalog(path, 0, kNeFSCatalogKindAlias));
	}

	/// @brief Creates a node with is a page file.
	/// @param path The filename path.
	/// @return The Node pointer.
	NodePtr NewFilesystemMgr::CreateSwapFile(const Char* path)
	{
		return node_cast(fImpl->CreateCatalog(path, 0, kNeFSCatalogKindPage));
	}

	/// @brief Gets the root directory.
	/// @return
	const Char* NewFilesystemHelper::Root()
	{
		return kNeFSRoot;
	}

	/// @brief Gets the up-dir directory.
	/// @return
	const Char* NewFilesystemHelper::UpDir()
	{
		return kNeFSUpDir;
	}

	/// @brief Gets the separator character.
	/// @return
	const Char NewFilesystemHelper::Separator()
	{
		return kNeFSSeparator;
	}

	/// @brief Gets the metafile character.
	/// @return
	const Char NewFilesystemHelper::MetaFile()
	{
		return kNeFSMetaFilePrefix;
	}
} // namespace Kernel

#endif // ifdef __FSKIT_USE_NEFS__
