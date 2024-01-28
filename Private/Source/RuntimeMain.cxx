/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include "NewKit/Defines.hpp"
#include <ArchKit/Arch.hpp>
#include <KernelKit/CodeManager.hpp>
#include <KernelKit/FileManager.hpp>
#include <NewKit/Json.hpp>

extern void (*__SYSTEM_INIT_END)();
extern void (**init)();

extern "C" void RuntimeMain()
{
    for (hCore::SizeT index_init = 0UL; init[index_init] != __SYSTEM_INIT_END; ++index_init)
    {
        init[index_init]();
    }

    MUST_PASS(hCore::init_hal());

    hCore::IFilesystemManager::Mount(new hCore::NewFilesystemManager());
    hCore::PEFLoader img("/System/Seeker.cm");

    if (!hCore::Utils::execute_from_image(img))
    {
        hCore::panic(RUNTIME_CHECK_BOOTSTRAP);
    }
}
