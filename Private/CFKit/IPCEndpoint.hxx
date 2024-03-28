/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef _INC_IPC_ENDPOINT_HXX_
#define _INC_IPC_ENDPOINT_HXX_

#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

/// @brief Includes macros and utilities to make an IPC connection.

/// IA separator.
#define kRemoteSeparator "."

/// Interchange address, consists of domain+namespace.
#define kRemoteInvalid "00.00.00.00:00000000"
#define kRemoteMaxLen 21

namespace NewOS {
    typedef UIntPtr ipc_method_type;
    typedef Char ipc_remote_type[kRemoteMaxLen];
} // namespace NewOS

#endif // _INC_IPC_ENDPOINT_HXX_