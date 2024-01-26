/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <Seeker/Seeker.hxx>

extern "C" Void   __InitCxxGlobals(Void);
extern "C" Void   __InitCommonCtrls(Void);
extern "C" Void   __InitSeekerCtrls(Void);
extern "C" char** __InitArgs(int* argc);

extern "C" Void  __InvokeMain(int argc, char** argv);

/* @brief As requested by PEF. */
extern "C" Void __start(Void)
{
    __InitCxxGlobals();
    __InitSeekerCtrls();
    __InitCommonCtrls();

    int argc = 0;
    auto argv = __InitArgs(&argc);

    __InvokeMain(argc, argv);
}