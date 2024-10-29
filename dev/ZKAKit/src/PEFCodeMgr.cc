/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#include <KernelKit/DebugOutput.h>
#include <KernelKit/Heap.h>
#include <KernelKit/PEFCodeMgr.h>
#include <KernelKit/UserProcessScheduler.h>
#include <NewKit/Defines.h>
#include <NewKit/Stop.h>
#include <NewKit/OwnPtr.h>
#include <NewKit/KString.h>

/// @brief PEF stack size symbol.
#define cPefStackSizeSymbol "SizeOfReserveStack"
#define cPefHeapSizeSymbol	"SizeOfReserveHeap"
#define cPefNameSymbol		"ProgramName"

namespace Kernel
{
	namespace Detail
	{
		/// @brief Get the PEF platform signature according to the compiled backebnd
		UInt32 ldr_get_platform(void) noexcept
		{
#if defined(__ZKA_32X0__)
			return kPefArch32x0;
#elif defined(__ZKA_64X0__)
			return kPefArch64x0;
#elif defined(__ZKA_AMD64__)
			return kPefArchAMD64;
#elif defined(__ZKA_PPC64__)
			return kPefArchPowerPC;
#elif defined(__ZKA_ARM64__)
			return kPefArchARM64;
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
		fFile.New(const_cast<Char*>(path), kRestrictRB);
		fPath = StringBuilder::Construct(path).Leak();

		auto cPefHeader = "PEF_CONTAINER";

		fCachedBlob = fFile->Read(cPefHeader);

		PEFContainer* container = reinterpret_cast<PEFContainer*>(fCachedBlob);

		if (container->Cpu == Detail::ldr_get_platform() &&
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
				 container->Magic[0] == kPefMagic[4] && container->Abi == kPefAbi)
		{
			/// This is a fat binary.
			this->fFatBinary = true;
			return;
		}

		fBad = true;

		if (fCachedBlob)
			mm_delete_heap(fCachedBlob);

		kcout << "PEFLoader: warn: Executable format error!\r";

		fCachedBlob = nullptr;
	}

	/// @brief PEF destructor.
	PEFLoader::~PEFLoader()
	{
		if (fCachedBlob)
			mm_delete_heap(fCachedBlob);

		fFile.Delete();
	}

	VoidPtr PEFLoader::FindSymbol(const Char* name, Int32 kind)
	{
		if (!fCachedBlob || fBad || !name)
			return nullptr;

		PEFContainer* container = reinterpret_cast<PEFContainer*>(fCachedBlob);

		auto blob = fFile->Read(name);

		PEFCommandHeader* container_header = reinterpret_cast<PEFCommandHeader*>(blob);

		constexpr auto cMangleCharacter	 = '$';
		const Char*	   cContainerKinds[] = {".code64", ".data64", ".zero64", nullptr};

		ErrorOr<KString> error_or_symbol;

		switch (kind)
		{
		case kPefCode: {
			error_or_symbol = StringBuilder::Construct(cContainerKinds[0]); // code symbol.
			break;
		}
		case kPefData: {
			error_or_symbol = StringBuilder::Construct(cContainerKinds[1]); // data symbol.
			break;
		}
		case kPefZero: {
			error_or_symbol = StringBuilder::Construct(cContainerKinds[2]); // block starting symbol.
			break;
		}
		default:
			return nullptr; // prevent that from the kernel's mode perspective, let that happen if it were
							// a user process.
		}

		Char* unconst_symbol = const_cast<Char*>(name);

		for (SizeT i = 0UL; i < rt_string_len(unconst_symbol, kPefNameLen); ++i)
		{
			if (unconst_symbol[i] == ' ')
			{
				unconst_symbol[i] = cMangleCharacter;
			}
		}

		error_or_symbol.Leak().Leak() += name;

		for (SizeT index = 0; index < container->Count; ++index)
		{
			if (StringBuilder::Equals(container_header->Name,
									  error_or_symbol.Leak().Leak().CData()))
			{
				if (container_header->Kind == kind)
				{
					if (container_header->Cpu != Detail::ldr_get_platform())
					{
						if (!this->fFatBinary)
						{
							mm_delete_heap(blob);
							return nullptr;
						}
					}

					Char* container_blob_value = new Char[container_header->Size];

					rt_copy_memory((VoidPtr)((Char*)blob + sizeof(PEFCommandHeader)), container_blob_value, container_header->Size);
					mm_delete_heap(blob);

					kcout << "PEFLoader: INFO: Load stub: " << container_header->Name << "!\r";

					return container_blob_value;
				}
			}
		}

		mm_delete_heap(blob);
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

	const Char* PEFLoader::Path()
	{
		return fPath.Leak().CData();
	}

	const Char* PEFLoader::AsString()
	{
#ifdef __32x0__
		return "32x0 PEF executable.";
#elif defined(__64x0__)
		return "64x0 PEF executable.";
#elif defined(__x86_64__)
		return "x86_64 PEF executable.";
#elif defined(__aarch64__)
		return "aarch64 PEF executable.";
#elif defined(__powerpc64__)
		return "POWER64 PEF executable.";
#else
		return "???? PEF executable.";
#endif // __32x0__ || __64x0__ || __x86_64__ || __powerpc64__
	}

	const Char* PEFLoader::MIME()
	{
		return kPefApplicationMime;
	}

	namespace Utils
	{
		ProcessID execute_from_image(PEFLoader& exec, const Int32& procKind) noexcept
		{
			auto errOrStart = exec.FindStart();

			if (errOrStart.Error() != kErrorSuccess)
				return No;

			UserProcess proc{};

			proc.Image		 = errOrStart.Leak().Leak();
			proc.Kind		 = procKind;
			proc.StackSize	 = *(UIntPtr*)exec.FindSymbol(cPefStackSizeSymbol, kPefData);
			proc.MemoryLimit = *(UIntPtr*)exec.FindSymbol(cPefHeapSizeSymbol, kPefData);

			rt_set_memory(proc.Name, 0, kProcessLen);

			if (exec.FindSymbol(cPefNameSymbol, kPefData))
				rt_copy_memory(exec.FindSymbol(cPefNameSymbol, kPefData), proc.Name, rt_string_len((Char*)exec.FindSymbol(cPefNameSymbol, kPefData)));

			if (!proc.StackSize)
			{
				const auto cDefaultStackSizeMib = 8;
				proc.StackSize					= mib_cast(cDefaultStackSizeMib);
			}

			return UserProcessScheduler::The().Add(proc);
		}
	} // namespace Utils
} // namespace Kernel
