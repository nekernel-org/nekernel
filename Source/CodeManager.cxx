/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/FileManager.hpp>
#include <KernelKit/CodeManager.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/ProcessManager.hpp>
#include <NewKit/KHeap.hpp>
#include <NewKit/String.hpp>
#include <NewKit/OwnPtr.hpp>
#include <NewKit/ErrorID.hpp>

namespace hCore
{
    namespace Detail
    {
        UInt32 rt_get_pef_platform(void) noexcept
        {
#ifdef __32x0__
            return kPefArch32x0;
#elif defined(__64x0__)
	    return kPefArch64x0;
#elif defined(__x86_64__)
            return kPefArchAMD64;
#else
            return kPefArchInvalid;
#endif // __POWER || __x86_64__
        }
    }

    PEFLoader::PEFLoader(const char* path)
        : fCachedBlob(nullptr), fBad(false)
    {
        OwnPtr<FileStream<char>> file;
		
        file.New(const_cast<Char*>(path));

        if (StringBuilder::Equals(file->MIME(), this->MIME()))
        {
        	fPath = StringBuilder::Construct(path).Leak();
        	
            fCachedBlob = file->ReadAll();

            PEFContainer* container = reinterpret_cast<PEFContainer*>(fCachedBlob);

            auto fFree = [&]() -> void {
                kcout << "CodeManager: Warning: Bad executable, program will not be started!\n";
                fBad = true;

                kernel_delete_ptr(fCachedBlob);
                
                fCachedBlob = nullptr;
            };

            if (container->Cpu == Detail::rt_get_pef_platform() &&
                container->Magic[0] == kPefMagic[0] &&
                container->Magic[1] == kPefMagic[1] &&
                container->Magic[2] == kPefMagic[2] &&
                container->Abi == kPefAbi)
            {
                if (container->Kind != kPefKindObject &&
                    container->Kind != kPefKindDebug)
                {
                    kcout << "CodeManager: Info: Good executable. can proceed.\n";
                    return;
                }
            }

            fFree();
        }
    }

    PEFLoader::~PEFLoader()
    {
        if (fCachedBlob)
            kernel_delete_ptr(fCachedBlob);
    }

    VoidPtr PEFLoader::FindSymbol(const char* name, Int32 kind)
    {
        if (!fCachedBlob ||
            fBad)
            return nullptr;

        PEFContainer* container = reinterpret_cast<PEFContainer*>(fCachedBlob);

        PEFCommandHeader* container_header = reinterpret_cast<PEFCommandHeader*>((UIntPtr)fCachedBlob + sizeof(PEFContainer));

        for (SizeT index = 0; index < container->Count; ++index)
        {
            kcout << "Iterating over container at index: " << StringBuilder::FromInt("%", index) << ", name: " << container_header->Name << "\n";

            if (StringBuilder::Equals(container_header->Name, name))
            {
                kcout << "Found potential container, checking for validity.\n";

                if (container_header->Kind == kind)
                    return static_cast<UIntPtr*>(fCachedBlob) + container_header->Offset;

                continue;
            }
        }

        return nullptr;
    }

    ErrorOr<VoidPtr> PEFLoader::LoadStart()
    {
        if (auto sym = this->FindSymbol("__start", kPefCode); sym)
            return ErrorOr<VoidPtr>(sym);

        return ErrorOr<VoidPtr>(ME_EXEC_ERROR);
    }

    bool PEFLoader::IsLoaded() noexcept { return !fBad && fCachedBlob; }

    namespace Utils
    {
        bool execute_from_image(PEFLoader& exec)
        {
            auto errOrStart = exec.LoadStart();

            if (errOrStart.Error() != 0)
                return false;

            Process proc(errOrStart.Leak().Leak());
            Ref<Process> refProc = proc;

            return ProcessManager::Shared().Leak().Add(refProc);
        }
    }

    const char* PEFLoader::Path() { return fPath.Leak().CData(); }

    const char* PEFLoader::Format() { return "PEF"; }

    const char* PEFLoader::MIME() { return "application/x-exec"; }
} // namespace hCore
