/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __SEEKER__
#define __SEEKER__

#include <CompilerKit/Compiler.hpp>

/* @brief Seeker SDK. */
/* Seeker helps with GUI developement. */

enum
{
    kRegularFile,
    kDirectory,
    kJunctionFile,
    kNetworkFile,
    kNetworkDirectory,
    kNetworkJunction,
    kDrive,
    kDevice,
};

enum
{
    kHardDrv    = 110,
    kOpticalDrv = 111,
    kUsbDrv     = 112,
};

enum
{
    kLangUs     = 113,
    kLangUk     = 114,
    kLangFr     = 115,
    kLangNl     = 116,
};

#define kResourceExt    ".rsc"
#define kExecutableExt  ".exe"
#define kLibExt         ".lib"
#define kShLibExt       ".dlib"

typedef void Void;
typedef void* VoidPtr;

typedef wchar_t WideChar;
typedef wchar_t* WideCString;

typedef __SIZE_TYPE__ SizeT;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SeekerProtocol;

// @brief Opens a Seeker window.
extern "C" VoidPtr SeekerOpenExplorer(const WideCString startCategory);

// @brief Closes a Seeker window.
extern "C" Void SeekerCloseExplorer(VoidPtr handle);

// @brief Make Seeker connect to endpoint.
extern "C" Void SeekerExplorerConnectToNetwork(VoidPtr handle,
                                                const WideCString endpoint, 
                                                struct SeekerProtocol* proto);

extern "C" Void SeekerExplorerChangeTo(VoidPtr handle, 
                                        const WideCString nodeName);

// @brief Search for elements inside directory/drive
// returns a class list as voidstar.
extern "C" VoidPtr SeekerSearch(VoidPtr handle, 
                                const WideCString where, 
                                const WideCString forWhat);

#define kProtocolStrLen 8

// @brief Seeker Location Protocol.
// @brief takes a handler class to handle server response.
struct SeekerProtocol
{
    WideChar fProtocol[kProtocolStrLen]; // ftp, sftp, http
    VoidPtr fProtocolHandlerClass;
    SizeT fProtocolHandlerClassSize;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void(*WndProcType)(int msg, VoidPtr lparam, VoidPtr wparam);

extern "C" bool    WindowClassCreate(const WideCString name, VoidPtr windowStruct, SizeT windowStructSz);
extern "C" VoidPtr WindowCreate(const WideCString name, const WideCString wndClass, VoidPtr parent);
extern "C" Void    WindowFieldSet(const VoidPtr win, const WideCString key, const WideCString value);
extern "C" Void    WindowFree(VoidPtr win);
extern "C" Void    WindowProcSet(VoidPtr win, void(*winproc)(int msg, VoidPtr lparam, VoidPtr wparam));
extern "C" Void    WindowFreeClass(VoidPtr classPtr);

// passing nullptr will give a console to the desktop.
extern "C" Void    OpenConsole(VoidPtr handle);
extern "C" Void    CloseConsole(VoidPtr handle);

// Runtime functions
extern "C" void    RtInitTerm(void);
extern "C" bool    RtIsValidHeapPtr(VoidPtr heap);
extern "C" Void    RtDialogAssert(const bool expr, const wchar_t* file, const wchar_t* expression);
extern "C" Void    RtShellOpen(const WideCString path, int argc, const WideCString* argv);

#define __WideExpr(x) L##x
#define RtWideString(x) __WideExpr(x)

#define RtAssert(expr) RtDialogAssert(expr, RtWideString(__FILE__), RtWideString(#expr))

typedef VoidPtr HandlePtr;
typedef HandlePtr IdPtr;

/////////////////////////////////////////////////////

HandlePtr SeekerAboutDlg(Void);

#define FINDER_YES L"Yes"
#define FINDER_NO  L"No"

#define kAssertInterrupt "sc $1"

extern "C" Void QuickDrawRect(float x, float y,
                              float r, float g, float b, 
                              float w, float h, float strike, 
                              float radius);

extern "C" Void QuickDrawPattern(float x, float y,
                               float* pattern, size_t sz,
                               float w, float h, float strike,
                               float radius);

// @brief a C++ class.
typedef VoidPtr SeekerClassPtr;

typedef struct 
{
    float x, y;
    float w, h;
} TAreaType;

/////////////////////////////////////////////////////

namespace Seeker
{
    class TControl
    {
    public:
        explicit TControl() = default;
        virtual ~TControl() = default;

        HCORE_COPY_DEFAULT(TControl);

    public:
        virtual void Draw() noexcept = 0;

    public:
        TAreaType fVisibleArea;
        TAreaType fFullArea;

    };
}
#endif /* ifndef __SEEKER__ */