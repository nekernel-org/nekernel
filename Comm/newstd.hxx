/* -------------------------------------------

Copyright Zeta Electronics Corporation.

File: newstd.hxx.
Purpose: NewOS standard interface.

------------------------------------------- */

#pragma once

#ifdef __KERNEL__
#error !!! including header in kernel mode !!!
#endif // __KERNEL__

#define IMPORT_CXX extern "C++"
#define IMPORT_C extern "C"

class NUser; /// @brief User application class.
class NWindow; /// @brief Window class.
class NWindowAlert; /// @brief Window alert object
class NURL; /// @brief URL object.

typedef void(*NWindowCallback)(NWindow*);

typedef int NOSType;
typedef bool NOSBool;

/**
@brief This contains has standard application functions.
*/
class NUser final
{
public:
    /// THOSE REQUIRES PERMISSIONS FROM THE USER. ///

    static void Poweroff();
    static void Reboot();

    /// THOSE REQUIRES PERMISSIONS FROM THE USER. ///

    static NOSBool IsWokeup();
    static void Terminate();

    /// THOSE DOESNT REQUIRE PERMISSIONS FROM THE USER. ///

    static NOSBool Exit(NOSType code);
    static void* New(long long sz);
    static void Delete(void* ptr);

    /// ASK FOR ELEVATION ///

    static NOSBool RaiseUAC();

    /// THOSE MAY REQUIRE PERMISSIONS FROM THE USER. ///

    static NOSType Open(const char* path);
    static void Close(NOSType fd);
    static NURL* Execute(const NURL* command);
    static void* Read(const char* forkName, NOSType fd);
    static void* Write(const char* forkName, NOSType fd);
};

/**
@brief Class which exposes the app alert API.
*/
class NWindowAlert
{
public:
    explicit NWindowAlert() = default;
    virtual ~NWindowAlert() = default;

public:
    /// @brief Opens an alert dialog.
    virtual NWindowAlert* Alert(const char* message, const char* title) = 0;

    /// @brief Makes a prompt dialog.
    virtual NWindowAlert* Prompt(const char* message, const char* title) = 0;

    /// @brief Makes a prompt notification dialog.
    virtual NWindowAlert* PromptNotification(const char* message, const char* title) = 0;

    /// @brief Makes a notification dialog.
    virtual NWindowAlert* Notification(const char* message, const char* title) = 0;

    /// Collect result of prompt/notification.
    virtual NWindowAlert* Collect(const char* resultBuf, long resultBufSz) = 0;

public:
    NOSBool fAsyncOperationMode;
    NWindowCallback fAsyncOnComplete;

};

/// @brief Window object.
class NWindow
{
public:
    explicit NWindow() = default;
    virtual ~NWindow() = default;

public:
    virtual NWindow* New(const char* pageName) = 0;

    virtual NWindow* Ref(NWindow* pagee) = 0;

    virtual NWindow* Text(const char* text) = 0;

    virtual NWindow* Button(const char* text, NWindowCallback onClick = nullptr,
							NWindowCallback onDblClick = nullptr) = 0;

    virtual NWindow* Checkbox(const char* text, NWindowCallback onSelect = nullptr,
							  NWindowCallback onUnselect = nullptr) = 0;

    virtual NWindow* Radio(const char* text, NWindowCallback onSelect = nullptr,
						   NWindowCallback onUnselect = nullptr) = 0;

    virtual NWindow* Link(const char* where, const char* textIfAny = "", NWindowCallback onClick = nullptr,
						  NWindowCallback onHover = nullptr) = 0;

public:
    NOSBool  fWindowEnabled;
    void* fWindowDataPtr;

};

/**
This class contains an URL
*/
class NURL
{
public:
    explicit NURL() = default;
    virtual ~NURL() = default;

public:
    virtual NURL* Navigate(const char* url) = 0;
    virtual NURL* Protocol(const char* protocol) = 0;

public:
    char* fBufferPtr;
    long fBufferLen;

};
