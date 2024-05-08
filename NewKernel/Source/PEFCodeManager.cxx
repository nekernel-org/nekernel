/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessScheduler.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/KernelCheck.hpp>
#include <NewKit/OwnPtr.hpp>
#include <NewKit/String.hpp>

namespace NewOS
{
	namespace Detail
	{
		/// @brief Get the PEF platform signature according to the compiled backebnd
		UInt32 rt_get_pef_platform(void) noexcept
		{
#ifdef __32x0__
			return kPefArch32x0;
#elif defined(__64x0__)
			return kPefArch64x0;
#elif defined(__x86_64__)
			return kPefArchAMD64;
#elif defined(__powerpc64__)
			return kPefArchPowerPC;
#else
			return kPefArchInvalid;
#endif // __32x0__ || __64x0__ || __x86_64__
		}
	} // namespace Detail

	/// @brief PEF loader constructor w/ blob.
	/// @param blob
	PEFLoader::PEFLoader(const VoidPtr blob)
		: fCachedBlob(blob)
	{
		MUST_PASS(fCachedBlob);
		fBad = false;
	}

	/// @brief PEF loader constructor.
	/// @param path the filesystem path.
	PEFLoader::PEFLoader(const Char* path)
		: fCachedBlob(nullptr), fBad(false), fFatBinary(false)
	{
		OwnPtr<FileStream<Char>> file;

		file.New(const_cast<Char*>(path), kRestrictRB);

		if (StringBuilder::Equals(file->MIME(), this->MIME()))
		{
			fPath = StringBuilder::Construct(path).Leak();

			fCachedBlob = file->Read();

			PEFContainer* container = reinterpret_cast<PEFContainer*>(fCachedBlob);

			if (container->Cpu == Detail::rt_get_pef_platform() &&
				container->Magic[0] == kPefMagic[0] &&
				container->Magic[1] == kPefMagic[1] &&
				container->Magic[2] == kPefMagic[2] &&
				container->Magic[3] == kPefMagic[3] &&
				container->Magic[4] == kPefMagic[4] && container->Abi == kPefAbi)
			{
				return;
			}
			else if (container->Magic[4] == kPefMagic[0] &&
					 container->Magic[3] == kPefMagic[1] &&
					 container->Magic[2] == kPefMagic[2] &&
					 container->Magic[1] == kPefMagic[3] &&
					 container->Magic[0] == kPefMagic[0] && container->Abi == kPefAbi)
			{
				/// This is a fat binary.
				this->fFatBinary = true;
				return;
			}

			kcout << "CodeManager: Warning: Executable format error!\n";
			fBad = true;

			ke_delete_ke_heap(fCachedBlob);

			fCachedBlob = nullptr;
		}
	}

	/// @brief PEF destructor.
	PEFLoader::~PEFLoader()
	{
		if (fCachedBlob)
			ke_delete_ke_heap(fCachedBlob);
	}

	VoidPtr PEFLoader::FindSymbol(const char* name, Int32 kind)
	{
		if (!fCachedBlob || fBad)
			return nullptr;

		PEFContainer* container = reinterpret_cast<PEFContainer*>(fCachedBlob);

		PEFCommandHeader* container_header = reinterpret_cast<PEFCommandHeader*>(
			(UIntPtr)fCachedBlob + sizeof(PEFContainer));

		constexpr auto cMangleCharacter	 = '$';
		const char*	   cContainerKinds[] = {".code64", ".data64", ".zero64", nullptr};

		ErrorOr<StringView> errOrSym;

		switch (kind)
		{
		case kPefCode: {
			errOrSym = StringBuilder::Construct(cContainerKinds[0]); // code symbol.
			break;
		}
		case kPefData: {
			errOrSym = StringBuilder::Construct(cContainerKinds[1]); // data symbol.
			break;
		}
		case kPefZero: {
			errOrSym = StringBuilder::Construct(cContainerKinds[2]); // block starting symbol.
			break;
		}
		default:
			return nullptr;
		}

		char* unconstSymbol = const_cast<char*>(name);

		for (SizeT i = 0UL; i < rt_string_len(unconstSymbol, kPefNameLen); ++i)
		{
			if (unconstSymbol[i] == ' ')
			{
				unconstSymbol[i] = cMangleCharacter;
			}
		}

		errOrSym.Leak().Leak() += name;

		for (SizeT index = 0; index < container->Count; ++index)
		{
			if (StringBuilder::Equals(container_header->Name,
									  errOrSym.Leak().Leak().CData()))
			{
				if (container_header->Kind == kind)
				{
					if (container_header->Cpu != Detail::rt_get_pef_platform())
					{
						if (!this->fFatBinary)
							return nullptr;
					}

					return (VoidPtr)(static_cast<UIntPtr*>(fCachedBlob) +
									 container_header->Offset);
				}
			}
		}

		return nullptr;
	}

	/// @brief Finds the executable entrypoint.
	/// @return
	ErrorOr<VoidPtr> PEFLoader::FindStart()
	{
		if (auto sym = this->FindSymbol(kPefStart, kPefCode); sym)
			return ErrorOr<VoidPtr>(sym);

		return ErrorOr<VoidPtr>(kErrorExecutable);
	}

	/// @brief Tells if the executable is loaded or not.
	/// @return
	bool PEFLoader::IsLoaded() noexcept
	{
		return !fBad && fCachedBlob;
	}

	namespace Utils
	{
		bool execute_from_image(PEFLoader& exec, const Int32& procKind) noexcept
		{
			auto errOrStart = exec.FindStart();

			if (errOrStart.Error() != 0)
				return false;

			ProcessHeader	   proc(errOrStart.Leak().Leak());
			Ref<ProcessHeader> refProc = proc;

			proc.Kind = procKind;

			return ProcessScheduler::Shared().Leak().Add(refProc);
		}
	} // namespace Utils

	const char* PEFLoader::Path()
	{
		return fPath.Leak().CData();
	}

	const char* PEFLoader::FormatAsString()
	{
#ifdef __32x0__
		return "32x0 PEF.";
#elif defined(__64x0__)
		return "64x0 PEF.";
#elif defined(__x86_64__)
		return "x86_64 PEF.";
#elif defined(__powerpc64__)
		return "POWER PEF.";
#else
		return "Unknown PEF.";
#endif // __32x0__ || __64x0__ || __x86_64__ || __powerpc64__
	}

	const char* PEFLoader::MIME()
	{
		return kPefApplicationMime;
	}
} // namespace NewOS
