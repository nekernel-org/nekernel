/* -------------------------------------------

Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef BENCHKIT_CHRONO_H
#define BENCHKIT_CHRONO_H

#include <CompilerKit/CompilerKit.h>
#include <NeKit/Defines.h>

/// @author Amlal El Mahrouss
/// @brief BenchKit Chrono contract.

#define BENCHKIT_INTERFACE : public ::Kernel::ChronoInterface

namespace Kernel {
class ChronoInterface;

/// @brief a Chronometer interface used for benchmarking.
class ChronoInterface {
 public:
  ChronoInterface()          = default;
  virtual ~ChronoInterface() = default;

  NE_COPY_DEFAULT(ChronoInterface)

  virtual Void   Start()                = 0;
  virtual Void   Stop()                 = 0;
  virtual Void   Reset()                = 0;
  virtual UInt64 GetElapsedTime() const = 0;
};
}  // namespace Kernel

namespace BenchKit {
using namespace Kernel;
}

#endif  // BENCHKIT_CHRONO_H
