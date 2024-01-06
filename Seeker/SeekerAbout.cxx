/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <Seeker/Seeker.hxx>

HandlePtr SeekerAboutDlg(Void)
{
    HandlePtr windowHandle = WindowCreate(L"hCore", L"OSAboutDialog", nullptr);
    return windowHandle;
}