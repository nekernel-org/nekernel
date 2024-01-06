/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <Seeker/Seeker.hxx>

extern "C" Void RtDialogAssert(const bool expr, const wchar_t* file, const wchar_t* expression)
{
    auto handle = WindowCreate(L"Assertion failed!", L"OSWndAssert", nullptr);

    WindowFieldSet(handle, L"Title", L"Assertion failed!");
    WindowFieldSet(handle, L"ButtonClicked_OK", L"Abort");

    asm (kAssertInterrupt);
}