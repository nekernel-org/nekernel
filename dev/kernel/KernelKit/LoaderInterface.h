/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <NeKit/Defines.h>
#include <NeKit/ErrorOr.h>
#include <hint/CompilerHint.h>

namespace Kernel {
/// @brief This interface is used to make loader contracts (MSCOFF, PEF).
/// @author @Amlal-El-Mahrouss
class LoaderInterface {
 public:
  explicit LoaderInterface() = default;
  virtual ~LoaderInterface() = default;

  NE_COPY_DEFAULT(LoaderInterface)

 public:
  virtual _Output ErrorOr<VoidPtr> GetBlob()                                              = 0;
  virtual _Output const Char* AsString()                                                  = 0;
  virtual _Output const Char* MIME()                                                      = 0;
  virtual _Output const Char* Path()                                                      = 0;
  virtual _Output ErrorOr<VoidPtr> FindStart()                                            = 0;
  virtual _Output ErrorOr<VoidPtr> FindSymbol(_Input const Char* name, _Input Int32 kind) = 0;
};
}  // namespace Kernel
