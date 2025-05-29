/* -------------------------------------------

Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef BENCHKIT_CHRONO_H
#define BENCHKIT_CHRONO_H

#include <CompilerKit/CompilerKit.h>
#include <NeKit/Defines.h>

namespace Kernel {
class ChronoInterface;

/// @brief a Chronometer interface used for benchmarking.
class ChronoInterface {
 public:
  ChronoInterface()          = default;
  virtual ~ChronoInterface() = default;

  NE_COPY_DEFAULT(ChronoInterface);

  virtual void   Start()                = 0;
  virtual void   Stop()                 = 0;
  virtual void   Reset()                = 0;
  virtual UInt64 GetElapsedTime() const = 0;
};
}  // namespace Kernel

#endif  // BENCHKIT_CHRONO_H
