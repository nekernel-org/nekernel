/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Dim2d.hpp
    Purpose:

    Revision History:

    31/01/24: Added file (amlel)

------------------------------------------- */

#ifndef __GORG__DIM2D_HPP__
#define __GORG__DIM2D_HPP__

#include <GKit/Core.hpp>

namespace HCore {
class Dim2d final {
 public:
  explicit Dim2d() = delete;
  explicit Dim2d(const HCore::UInt& x, const HCore::UInt& y) : m_X(x), m_Y(y) {}

  Dim2d& operator=(const Dim2d&) = default;
  Dim2d(const Dim2d&) = default;

  HCore::UInt& X();
  HCore::UInt& Y();

 private:
  HCore::UInt m_X{0};
  HCore::UInt m_Y{0};
};
}  // namespace HCore

#endif /* ifndef __GORG__DIM2D_HPP__ */
