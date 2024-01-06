/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/FileManager.hpp>
#include <KernelKit/CodeManager.hpp>
#include <ArchKit/Arch.hpp>

extern "C" void __KernelMain(hCore::VoidPtr this_image)
{
    hCore::Detail::serial_init();
    MUST_PASS(hCore::initialize_hardware_components());

    hCore::IFilesystemManager::Mount(new hCore::NewFilesystemManager());
    hCore::PEFLoader img("/System/Seeker");

    if (!hCore::Utils::execute_from_image(img))
    {
        hCore::panic(RUNTIME_CHECK_BOOTSTRAP);
    }
}
