#!/bin/sh

# Connect to QEMU's debug server, same as boot/gdbinit
lldb -o "gdb-remote 1234"

