/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Core.hxx
    Purpose:

    Revision History:

    31/01/24: Added file (amlel)
    08/02/24: Update Form to GForm. (amlel)

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>
#include <NewKit/String.hpp>

#define $() HCore::GApplication::Shared()->Document()

#ifdef __IMPORT_LIB
#define G_API __attribute__((pef_container(".IMPORT")))
#else
#define G_API __attribute__((pef_container(".EXPORT")))
#endif  // ifdef __EXPORT_LIB

namespace System::Graphics {
template <typename... T>
class GAction;

class GString;
class GNumber;
class GVector2;
class GBoolean;
class GDocument;
class GApplication;
class GFrame;

class G_API GBoolean {
 private:
  explicit GBoolean() : m_Value(false) {}

  HCore::Boolean m_Value;

  friend class GForm;

 public:
  static const GBoolean Construct(HCore::StringView& sw, HCore::Boolean value) {
    GBoolean boolean;
    boolean.m_Value = value;

    return boolean;
  }
};

template <typename... T>
class G_API GAction {
  explicit GAction(HCore::StringView& sw) { m_Name = sw; }

  HCore::StringView m_Name;
  void (*m_Action)(T&&... args);

  friend class GForm;

 public:
  static const GAction Construct(HCore::StringView& sw,
                                 void (*action)(T&&... args)) {
    GAction actcls{sw};
    actcls.m_Action = action;

    return actcls;
  }
};

class G_API GVector2 {
  explicit GVector2(HCore::StringView& sw) : m_Vec2() {}

  HCore::Array<HCore::Int, 3> m_Vec2;

  friend class GForm;

 public:
  static const GVector2 Construct(HCore::StringView& sw,
                                  HCore::Array<HCore::Int, 3>& vec2) {
    GVector2 vec{sw};
    vec.m_Vec2 = vec2;

    return vec;
  }
};

class G_API GNumber {
  HCore::Int m_Number{0};
  friend class GForm;

 public:
  static const GNumber Construct(HCore::Int& number) {
    GNumber num;
    num.m_Number = number;

    return num;
  }
};

class G_API GString {
  explicit GString(HCore::StringView& content) {
    m_Content = new HCore::StringView();
    *m_Content = content;
  }

  HCore::StringView* m_Content;

  friend class GForm;

 public:
  static const GString Construct(HCore::StringView& value) {
    GString str{value};
    return str;
  }
};

class G_API GApplication final {
 public:
  explicit GApplication() = default;
  ~GApplication() = default;

  HCORE_COPY_DEFAULT(GApplication);

  GDocument* Document() noexcept { return nullptr; }

  static GApplication* Shared() noexcept {
    STATIC GApplication* kApp = nullptr;

    if (!kApp) kApp = new GApplication();

    return kApp;
  }
};

class G_API GDocument final {
 public:
  explicit GDocument(HCore::StringView& sv) : mString(GString::Construct(sv)) {}
  ~GDocument() = default;

  HCORE_COPY_DEFAULT(GDocument);

  GFrame** GetAddressOf() noexcept { return &mFrame; }
  GFrame* Get() noexcept { return mFrame; }

  GString& Name() { return mString; }

 private:
  GFrame* mFrame{nullptr};
  GString mString;
};

class GException final {
 public:
  explicit GException() = default;
  ~GException() = default;

 public:
  HCORE_COPY_DEFAULT(GException);

 public:
  const char* Name() { return "GUI Error"; }
  const char* Reason() { return mReason; }

 private:
  const char* mReason{"CoreAPI: GUI Exception!"};
};

template <typename GFrameType, typename GFrameBase>
inline GFrameType* frame_cast(GFrameBase* Frame) {
  if (!dynamic_cast<GFrameType*>(Frame)) {
    throw GException();
  }

  return dynamic_cast<GFrameType*>(Frame);
}
}  // namespace System::Graphics
