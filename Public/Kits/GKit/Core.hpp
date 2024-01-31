/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Core.hpp
    Purpose:

    Revision History:

    31/01/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>
#include <NewKit/String.hpp>

namespace HCore {
template <typename... T>
class GAction;

class GString;
class GNumber;
class GVector2;
class GBoolean;

class GBoolean {
 private:
  explicit GBoolean() : m_Value(false) {}

  HCore::Boolean m_Value;

  friend class Form;

 public:
  static const GBoolean Construct(HCore::StringView& sw, HCore::Boolean value) {
    GBoolean boolean;
    boolean.m_Value = value;

    return boolean;
  }
};

template <typename... T>
class GAction {
  explicit GAction(HCore::StringView& sw) { m_Name = sw; }

  HCore::StringView m_Name;
  void (*m_Action)(T&&... args);

  friend class Form;

 public:
  static const GAction Construct(HCore::StringView& sw,
                                 void (*action)(T&&... args)) {
    GAction actcls{sw};
    actcls.m_Action = action;

    return actcls;
  }
};

class GVector2 {
  explicit GVector2(HCore::StringView& sw) : m_Vec2() {}

  HCore::Array<HCore::Int, 3> m_Vec2;

  friend class Form;

 public:
  static const GVector2 Construct(HCore::StringView& sw,
                                  HCore::Array<HCore::Int, 3>& vec2) {
    GVector2 vec{sw};
    vec.m_Vec2 = vec2;

    return vec;
  }
};

class GNumber {
  HCore::Int m_Number{0};
  friend class Form;

 public:
  static const GNumber Construct(HCore::Int& number) {
    GNumber num;
    num.m_Number = number;

    return num;
  }
};

class GString {
  explicit GString(HCore::StringView& content) {
    m_Content = new HCore::StringView();
    *m_Content = content;
  }

  HCore::StringView* m_Content;

  friend class Form;

 public:
  static const GString Construct(HCore::StringView& value) {
    GString str{value};
    return str;
  }
};
}  // namespace HCore
