/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#pragma once

#include <System.Core/CoreAPI.hxx>

typedef struct HcObject {
  void(*Release)(void);
  void(*Invoke)(void);
  void(*QueryInterface)(void);
} *HcObjectPtr;
