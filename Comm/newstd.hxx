/* -------------------------------------------

	Copyright Zeta Electronics Corporation.

	File: newstd.hxx.
	Purpose: NewOS standard interface.

------------------------------------------- */

#pragma once

#ifdef __KERNEL__
#error !!! including header in kernel mode !!!
#endif // __KERNEL__

class NUser; /// @brief User application class.
class NWindow; /// @brief Window class.
class NWindowAlert;

#define IMPORT_CXX extern "C++"
#define IMPORT_C extern "C"

/**
@brief Class which exposes the app I/O.
*/
class NUser final
{
public:
    /// THOSE REQUIRES PERMISSIONS FROM THE USER. ///

    static void Poweroff();
    static void Reboot();

    /// THOSE REQUIRES PERMISSIONS FROM THE USER. ///

    static bool IsWokeup();
    static void Terminate();

    /// THOSE DOESNT REQUIRE PERMISSIONS FROM THE USER. ///

    static bool Exit(int code);
    static void* New(long long sz);
    static void Delete(void* ptr);

    /// THOSE MAY REQUIRE PERMISSIONS FROM THE USER. ///

    static int Open(const char* path);
    static void Close(int fd);
    static void Execute(const char* command);
    static void* Read(const char* forkName, int fd);
    static void* Write(const char* forkName, int fd);
};

typedef void(*NWindowCallback)(NWindow*);

/**
@brief Class which exposes the app UI API.
*/
class NWindowAlert
{
public:
    virtual NWindowAlert* Alert(const char* message, const char* title) = 0;
    virtual NWindowAlert* Prompt(const char* message, const char* title) = 0;

    virtual NWindowAlert* Collect(const char* resultBuf, long resultBufSz) = 0;

public:
    bool fAsyncOperationMode;
    NWindowCallback fAsyncOnComplete;

};

class NWindow
{
public:
    virtual NWindow* New(const char* pageName) = 0;
    virtual NWindow* Ref(NWindow* pagee) = 0;

    virtual NWindow* Button(const char* text, NWindowCallback onClick = nullptr,
        NWindowCallback onDblClick = nullptr) = 0;

    virtual NWindow* Checkbox(const char* text, NWindowCallback onSelect = nullptr,
            NWindowCallback onUnselect = nullptr) = 0;

    virtual NWindow* Radio(const char* text, NWindowCallback onSelect = nullptr,
        NWindowCallback onUnselect = nullptr) = 0;

    virtual NWindow* Link(const char* where, const char* textIfAny = "", NWindowCallback onClick = nullptr,
        NWindowCallback onHover = nullptr) = 0;

public:
    bool fEnabled;

};
