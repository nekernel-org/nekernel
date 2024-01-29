/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <HintKit/CompilerHint.hxx>
#include <KernelKit/OSErr.hpp>
#include <NewKit/Utils.hpp>

using namespace hCore;

// unimplemented _HintTell
void _HintTell(_HintMessage* ppMsg, _HintId* pId) {
  switch (*pId) {
    case kErrorExecutable: {
      const char* msg = "CodeManager doesn't recognize this executable.";
      rt_copy_memory((const voidPtr)msg, *ppMsg, string_length(msg));
      break;
    }
    case kErrorExecutableLib: {
      const char* msg = "CodeManager doesn't recognize this library.";
      rt_copy_memory((const voidPtr)msg, *ppMsg, string_length(msg));
      break;
    }
    case kErrorFileNotFound: {
      const char* msg = "FileManager doesn't find this file.";
      rt_copy_memory((const voidPtr)msg, *ppMsg, string_length(msg));
      break;
    }
    case kErrorNoNetwork: {
      const char* msg =
          "NetworkManager doesn't detect any WiFi/Ethernet connection.";
      rt_copy_memory((const voidPtr)msg, *ppMsg, string_length(msg));
      break;
    }
    default:
      break;
  }
}
