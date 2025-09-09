/* -------------------------------------------

   Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

   ------------------------------------------- */

 .text

.extern nelaunch_startup_fn

_NeMain:
  push %rbp
  movq %rsp, %rbp
  callq nelaunch_startup_fn
  popq %rbp

  retq
