/*
 *      ========================================================
 *
 *      NewBoot
 *      Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#pragma once

#include <efi.h>
#include <efilib.h>

/// @brief auto-mount and boots from a bootable drive.
/// does not return on success.
void newboot_mount_drive(const char* name);
void newboot_boot_file(const char* path);

/// @brief initializes xpm library.
void newboot_init_epm(void);

/// @brief frees the xpm library, called when newboot_auto_mount/newboot_boot_file
/// succeeds.
void newboot_fini_xpm(void);
