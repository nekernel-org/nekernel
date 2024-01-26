/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <CompilerKit/Compiler.hpp>
#include <NewKit/Defines.hpp>

namespace h-core
{
    // h-core HardWare Interface.
    // Purpose: Provides a gentle way to initialize, dispose, send and receive hardware data.
    class HWInterface
    {
    public:
        HWInterface() = default;
        virtual ~HWInterface() = default;

    public:
        HCORE_COPY_DEFAULT(HWInterface);

    public:
        virtual const char* Name() { return "Unimplemented Hardware."; }

    public:
        virtual void Initialize() {}
        virtual void Dispose() {}

    public:
        virtual void Send(voidPtr msg, SizeT sz) { (void)msg; (void)sz; }
        virtual void Recv(voidPtr msg, SizeT sz) { (void)msg; (void)sz; }

    protected:
        virtual void OnSend(voidPtr msg, SizeT sz) { (void)msg; (void)sz; }
        virtual void OnRecv(voidPtr msg, SizeT sz) { (void)msg; (void)sz; }

    };
}
