/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#ifndef _INC_CODE_MANAGER_PEF_
#define _INC_CODE_MANAGER_PEF_

#include <KernelKit/PEF.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/String.hpp>

#define kPefApplicationMime "application/x-newos-exec"

namespace NewOS
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
		const char* Path() override;
		const char* FormatAsString() override;
		const char* MIME() override;

	public:
		ErrorOr<VoidPtr> FindStart() override;
		VoidPtr			 FindSymbol(const char* name, Int32 kind) override;

	public:
		bool IsLoaded() noexcept;

	private:
		Ref<StringView> fPath;
		VoidPtr			fCachedBlob;
		bool			fFatBinary;
		bool			fBad;
	};

	namespace Utils
	{
		bool execute_from_image(PEFLoader& exec, const Int32& procKind) noexcept;
	} // namespace Utils
} // namespace NewOS

#endif // ifndef _INC_CODE_MANAGER_PEF_
