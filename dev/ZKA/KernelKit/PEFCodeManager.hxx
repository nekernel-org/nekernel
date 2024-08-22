/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef _INC_CODE_MANAGER_PEF_HXX_
#define _INC_CODE_MANAGER_PEF_HXX_

#include <KernelKit/PEF.hxx>
#include <NewKit/ErrorOr.hxx>
#include <NewKit/String.hxx>
#include <KernelKit/FileManager.hxx>

#define kPefApplicationMime "application/x-newos-exec"

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
		NEWOS_COPY_DEFAULT(PEFLoader);

	public:
		const Char* Path() override;
		const Char* AsString() override;
		const Char* MIME() override;

	public:
		ErrorOr<VoidPtr> FindStart() override;
		VoidPtr			 FindSymbol(const Char* name, Int32 kind) override;

	public:
		bool IsLoaded() noexcept;

	private:
		OwnPtr<FileStream<Char>> fFile;
		Ref<StringView>			 fPath;
		VoidPtr					 fCachedBlob;
		bool					 fFatBinary;
		bool					 fBad;
	};

	namespace Utils
	{
		bool execute_from_image(PEFLoader& exec, const Int32& procKind) noexcept;
	} // namespace Utils
} // namespace Kernel

#endif // ifndef _INC_CODE_MANAGER_PEF_HXX_
