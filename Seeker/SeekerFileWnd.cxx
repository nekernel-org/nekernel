/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <Seeker/Seeker.hxx>

// Source code of the Seeker Window.

// @brief Opens a Seeker window.
extern "C" VoidPtr SeekerOpenExplorer(const WideCString startCategory)
{
    auto win = WindowCreate(L"Seeker", L"OSSeekerWnd", nullptr);

    if (win)
    {
        WindowFieldSet(win, L"Category", startCategory);
        WindowFieldSet(win, L"CanClose", FINDER_NO);
        WindowFieldSet(win, L"ClosePolicy", L"Restart");

        return win;
    }

    RtAssert(win != nullptr);

    return nullptr;
}

// @brief Closes a Seeker window.
extern "C" Void SeekerCloseExplorer(VoidPtr handle)
{
    if (RtIsValidHeapPtr(handle))
    {
        WindowFree(handle);
    }
}

extern "C" Void SeekerExplorerConnectToNetwork(VoidPtr handle,
                                               const WideCString endpoint, 
                                               struct SeekerProtocol* proto)
{
    if (handle)
    {
        WindowFieldSet(handle, L"NetworkProtocolEndpoint", endpoint);
        WindowFieldSet(handle, L"NetworkProtocolClass", (wchar_t*)proto->fProtocolHandlerClass);
        WindowFieldSet(handle, L"NetworkProtocolClassSize", (wchar_t*)proto->fProtocolHandlerClassSize);

        WindowFieldSet(handle, L"NetworkProtocolFire", FINDER_YES);
    }
}