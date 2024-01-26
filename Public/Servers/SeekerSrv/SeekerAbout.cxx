/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <SeekerSrv/Seeker.hxx>

HandlePtr SeekerAboutDlg(Void)
{
    HandlePtr windowHandle = WindowCreate(L"hCore", L"OSAboutDialog", nullptr);
    return windowHandle;
}