/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/MP.hxx>

namespace Kernel::Detail
{
    /// \brief Process thread information.
    struct THREAD_HEADER_BLOCK final
    {
        STATIC constexpr SizeT cMaxLen = 256;

        // Status
        Char fName[cMaxLen] = { "THREAD #0 (PROCESS 0)" };

        ProcessStatus fThreadStatus;

        // Information
        Int64 fThreadID;

        // Code buffers.
        UIntPtr fCode;
        UIntPtr fStack;
        UIntPtr fBSS;
        UIntPtr fProcessheader;

        // GX buffers.
        UIntPtr fTGB;
        UIntPtr fTGBSize;
    };
}
