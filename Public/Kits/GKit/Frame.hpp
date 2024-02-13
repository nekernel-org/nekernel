/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Frame.hpp
    Purpose:

    Revision History:

    31/01/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <GKit/Core.hpp>
#include <GKit/Dim2d.hpp>
#include <NewKit/MutableArray.hpp>

namespace HCore {
class G_API GFrame {
 public:
  explicit GFrame(GFrame* parent = nullptr) : m_ParentFrame(parent) {}
  ~GFrame() {}

  GFrame& operator=(const GFrame&) = default;
  GFrame(const GFrame&) = default;

  virtual void Update() {
    if (m_Frames.Count() == 0) return;

    for (int x = 0; x < m_Frames.Count(); ++x) {
      if (!m_Frames[x]->ShouldBeUpdated()) continue;

      m_Frames[x]->Update();
    }

    this->Paint();
  }

  virtual void UpdateInput() {
    if (m_Frames.Count() == 0) return;

    for (int x = 0; x < m_Frames.Count(); ++x) {
      if (!m_Frames[x]->ShouldBeUpdated()) continue;

      m_Frames[x]->UpdateInput();
    }
  }

  virtual bool ShouldBeUpdated() { return false; }

  virtual void Paint() = 0;

 private:
  HCore::MutableArray<GFrame*> m_Frames;
  GFrame* m_ParentFrame{nullptr};
};
}  // namespace HCore
