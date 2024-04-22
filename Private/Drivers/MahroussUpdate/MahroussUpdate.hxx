/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <DriverKit/KernelString.h>

#include "DriverKit/KernelPrint.h"

/// @file Software update job driver.

class UpdateRequest;
class UpdateRequestObserver;

class UpdateRequest {
 public:
  explicit UpdateRequest(const char* patchUrl = "mup://invalid-url-scheme/") {
    kernelStringCopy(this->fPatchUrl, patchUrl, kernelStringLength(patchUrl));

    kernelPrintStr("Mahrouss Update, Looking at: ");
    kernelPrintStr(patchUrl);
    kernelPrintChar('\r');
    kernelPrintChar('\n');
  }

  ~UpdateRequest() {}

 private:
  char fPatchUrl[4096] = {0};
};
