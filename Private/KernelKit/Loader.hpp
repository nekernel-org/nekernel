/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/ErrorOr.hpp>
#include <NewKit/Defines.hpp>
#include <CompilerKit/CompilerKit.hpp>

namespace HCore
{
    class Loader
    {
    public:
        Loader() = default;
        virtual ~Loader() = default;

        HCORE_COPY_DEFAULT(Loader);

    public:
        virtual const char* Format() = 0;
        virtual const char* MIME() = 0;
		virtual const char* Path() = 0;
        virtual ErrorOr<VoidPtr> LoadStart() = 0;
        virtual VoidPtr FindSymbol(const char* name, Int32 kind) = 0;

    };
}
