#include <public/frameworks/KernelTest.fwrk/headers/KernelTest.h>

EXTERN_C Kernel::Void KT_TestBreak()
{
    KT_ALWAYS_BREAK brk;
    brk.Run();
}

EXTERN_C Kernel::Void KT_TestGood()
{
    KT_ALWAYS_GOOD good;
    good.Run();
}