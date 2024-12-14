/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef _INC_CODE_MANAGER_PEF_H_
#define _INC_CODE_MANAGER_PEF_H_

#include <KernelKit/PEF.h>
#include <NewKit/ErrorOr.h>
#include <NewKit/KString.h>
#include <KernelKit/FileMgr.h>

#ifndef INC_PROCESS_SCHEDULER_H
#include <KernelKit/UserProcessScheduler.h>
#endif

#define kPefApplicationMime "application/vnd-zka-executable"

namespace Kernel
{
	///
	/// \name PEFLoader
	/// \brief PEF loader class.
	///
	class PEFLoader : public LoaderInterface
	{
	private:
		explicit PEFLoader() = delete;

	public:
		explicit PEFLoader(const VoidPtr blob);
		explicit PEFLoader(const Char* path);
		~PEFLoader() override;

	public:
		ZKA_COPY_DEFAULT(PEFLoader);

	public:
		const Char* Path() override;
		const Char* AsString() override;
		const Char* MIME() override;

	public:
		ErrorOr<VoidPtr> FindStart() override;
		VoidPtr			 FindSymbol(const Char* name, Int32 kind) override;
		ErrorOr<VoidPtr> GetBlob() override;

	public:
		bool IsLoaded() noexcept;

	private:
#ifdef __FSKIT_INCLUDES_NEFS__
		OwnPtr<FileStream<Char, NeFileSystemMgr>> fFile;
#else
		OwnPtr<FileStream<Char>> fFile;
#endif // __FSKIT_INCLUDES_NEFS__

		Ref<KString> fPath;
		VoidPtr		 fCachedBlob;
		bool		 fFatBinary;
		bool		 fBad;
	};

	namespace Utils
	{
		ProcessID rtl_create_process(PEFLoader& exec, const Int32& procKind) noexcept;
	} // namespace Utils
} // namespace Kernel

#endif // ifndef _INC_CODE_MANAGER_PEF_H_
