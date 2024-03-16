/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#pragma once

#include <System.Core/Defs.hxx>

typedef struct HcObject {
  void(*Release)(void);
  void(*Invoke)(void);
  void(*QueryInterface)(void);
} *HcObjectPtr;
