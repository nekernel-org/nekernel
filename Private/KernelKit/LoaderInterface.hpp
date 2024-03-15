/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>
#include <NewKit/ErrorOr.hpp>

namespace HCore {
/// @brief This interface is used to make loader contracts (MSCOFF, PEF).
/// @author @Amlal-El-Mahrouss
class LoaderInterface {
 public:
  explicit LoaderInterface() = default;
  virtual ~LoaderInterface() = default;

  HCORE_COPY_DEFAULT(LoaderInterface);

 public:
  virtual const char* Format() = 0;
  virtual const char* MIME() = 0;
  virtual const char* Path() = 0;
  virtual ErrorOr<VoidPtr> FindStart() = 0;
  virtual VoidPtr FindSymbol(const char* name, Int32 kind) = 0;
};
}  // namespace HCore
