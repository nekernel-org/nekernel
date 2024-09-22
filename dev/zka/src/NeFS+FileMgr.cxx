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
	NeFileSystemMgr::NeFileSystemMgr()
	{
		MUST_PASS(Detail::fs_init_newfs());
		fImpl = mm_new_class<NeFSParser>();
		MUST_PASS(fImpl);

		kcout << "We are done here... (NeFileSystemMgr).\r";
	}

	NeFileSystemMgr::~NeFileSystemMgr()
	{
		if (fImpl)
		{
			kcout << "Destroying FS class (NeFS)...\r";

			delete fImpl;
			fImpl = nullptr;
		}
	}

	/// @brief Removes a node from the filesystem.
	/// @param fileName The filename
	/// @return If it was deleted or not.
	bool NeFileSystemMgr::Remove(const Char* fileName)
	{
		if (fileName == nullptr || *fileName == 0)
			return false;

		return fImpl->RemoveCatalog(fileName);
	}

	/// @brief Creates a node with the specified.
	/// @param path The filename path.
	/// @return The Node pointer.
	NodePtr NeFileSystemMgr::Create(const Char* path)
	{
		return node_cast(fImpl->CreateCatalog(path));
	}

	/// @brief Creates a node with is a directory.
	/// @param path The filename path.
	/// @return The Node pointer.
	NodePtr NeFileSystemMgr::CreateDirectory(const Char* path)
	{
		return node_cast(fImpl->CreateCatalog(path, 0, kNeFSCatalogKindDir));
	}

	/// @brief Creates a node with is a alias.
	/// @param path The filename path.
	/// @return The Node pointer.
	NodePtr NeFileSystemMgr::CreateAlias(const Char* path)
	{
		return node_cast(fImpl->CreateCatalog(path, 0, kNeFSCatalogKindAlias));
	}

	/// @brief Creates a node with is a page file.
	/// @param path The filename path.
	/// @return The Node pointer.
	NodePtr NeFileSystemMgr::CreateSwapFile(const Char* path)
	{
		return node_cast(fImpl->CreateCatalog(path, 0, kNeFSCatalogKindPage));
	}

	/// @brief Gets the root directory.
	/// @return
	const Char* NeFileSystemHelper::Root()
	{
		return kNeFSRoot;
	}

	/// @brief Gets the up-dir directory.
	/// @return
	const Char* NeFileSystemHelper::UpDir()
	{
		return kNeFSUpDir;
	}

	/// @brief Gets the separator character.
	/// @return
	const Char NeFileSystemHelper::Separator()
	{
		return kNeFSSeparator;
	}

	/// @brief Gets the metafile character.
	/// @return
	const Char NeFileSystemHelper::MetaFile()
	{
		return kNeFSMetaFilePrefix;
	}
} // namespace Kernel

#endif // ifdef __FSKIT_USE_NEFS__
