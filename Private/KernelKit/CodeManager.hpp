/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_CODE_MANAGER_
#define _INC_CODE_MANAGER_

#include <KernelKit/PEF.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/String.hpp>

#include "NewKit/Defines.hpp"

namespace HCore {
///
/// \name PEFLoader
/// \brief PEF loader class.
///
class PEFLoader : public Loader {
 private:
  explicit PEFLoader() = delete;

 public:
  explicit PEFLoader(const VoidPtr blob);
  explicit PEFLoader(const char *path);
  ~PEFLoader() override;

 public:
  HCORE_COPY_DEFAULT(PEFLoader);

 public:
  typedef void (*MainKind)(void);

 public:
  const char *Path() override;
  const char *Format() override;
  const char *MIME() override;

 public:
  ErrorOr<VoidPtr> LoadStart() override;
  VoidPtr FindSymbol(const char *name, Int32 kind) override;

 public:
  bool IsLoaded() noexcept;

 private:
  Ref<StringView> fPath;
  VoidPtr fCachedBlob;
  bool fBad;
};

namespace Utils {
/// \brief Much like Mac OS's UPP.
/// This is read-only by design.
/// It handles different kind of code.
/// ARM <-> AMD64 for example.
typedef struct UniversalProcedureTable final {
  const Char NAME[kPefNameLen];
  const VoidPtr TRAP;
  const SizeT ARCH;
} __attribute__((packed)) UniversalProcedureTableType;

bool execute_from_image(PEFLoader &exec) noexcept;
}  // namespace Utils
}  // namespace HCore

#endif  // ifndef _INC_CODE_MANAGER_
