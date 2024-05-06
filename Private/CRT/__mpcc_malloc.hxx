/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

namespace stdx
{
/// @brief allocate a new class.
/// @tparam KindClass the class type to allocate.
template <typename KindClass, typename... Args>
inline NewOS::VoidPtr allocate(Args&&... args)
{
    return new KindClass(NewOS::forward(args)...);
}

/// @brief free a class.
/// @tparam KindClass the class type to allocate.
template <typename KindClass>
inline NewOS::Void release(KindClass ptr)
{
    if (!ptr) return;
    delete ptr;
}
} // namespace stdx
