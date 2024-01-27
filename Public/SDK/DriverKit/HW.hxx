/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <Private/CompilerKit/Compiler.hpp>
#include <Private/NewKit/Defines.hpp>

namespace h-core
{
    /// @brief Provides a gentle way to initialize, dispose, send and receive hardware data.
    class HardwareInterface
    {
    public:
        explicit HardwareInterface() ={ this->Initialize(); }
        virtual ~HardwareInterface() { this->Dispose(); }

    public:
        HCORE_COPY_DEFAULT(HardwareInterface);

    public:
        virtual const char* Name() { return "Generic HW."; }

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

// last rev 1/27/24