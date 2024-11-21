/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.h>
#include <Modules/FB/FB.h>
#include <Modules/FB/Text.h>
#include <NewKit/Utils.h>

/// @file WS.h
/// @brief WindowServer's endpoint implementation. (within the zka-dev subsystem)

namespace WS
{
    using namespace Kernel;

    struct WSWindowContainer;

    typedef UInt32* WSBackBufferKind;

    class WSWindowContainer final
    {
    public:
        WSWindowContainer() = default;
        ~WSWindowContainer() = default;

        ZKA_COPY_DEFAULT(WSWindowContainer);

        /// @note the trick is, it could be GPU processed data, that's the cool thing.
        BOOL PopulateWindow(WSBackBufferKind contents_buf, SizeT contents_len)
        {
            if (contents_len > BackBufferLength)
                return NO;

            if (!contents_buf)
                return NO;

            if (!BackBuffer ||
                !BackBufferLength)
                return NO;

            rt_copy_memory(contents_buf, BackBuffer, contents_len);
            return YES;
        }

    public:
        WSBackBufferKind BackBuffer{nullptr};
        SizeT BackBufferLength{0UL};

    };
}

#define rtl_allocate_backbuffer(width, height) new WS::WSBackBufferKind[width * height]
#define rtl_compute_fb_geometry(width, height) (width * height)
