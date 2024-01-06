/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <KernelKit/Device.hpp>
#include <NewKit/OwnPtr.hpp>
#include <NewKit/Stream.hpp>

namespace hCore
{
    // @brief Emulates a VT100 terminal.
    class TerminalDevice final : public IDevice<const char*>
    {
    public:
        TerminalDevice(void (*print)(const char *), void (*get)(const char *)) : IDevice<const char*>(print, get) {}
        virtual ~TerminalDevice() {}

        /// @brief returns device name (terminal name)
        /// @return string type (const char*)
        virtual const char* Name() const override { return ("TerminalDevice"); }

        TerminalDevice &operator=(const TerminalDevice &) = default;
        TerminalDevice(const TerminalDevice &) = default;

    };

    namespace Detail
    {
        bool serial_init();
    }

    extern TerminalDevice kcout;
} // namespace hCore
