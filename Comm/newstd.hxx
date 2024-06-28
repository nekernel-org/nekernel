/* -------------------------------------------

Copyright Zeta Electronics Corporation.

File: newstd.hxx.
Purpose: NewOS standard system call interface.

------------------------------------------- */

#pragma once

#ifdef __KERNEL__
#error !!! including header in kernel mode !!!
#endif // __KERNEL__

#define ML_IMPORT_CXX extern "C++"
#define ML_IMPORT_C	  extern "C"

#define cRestrictR	"r"
#define cRestrictRB "rb"
#define cRestrictW	"w"
#define cRestrictRW "rw"

class NUser;		/// @brief User application class.
class NWindow;		/// @brief Window class.
class NWindowAlert; /// @brief Window alert object
class NURL;			/// @brief URL object.

typedef void (*NWindowCallback)(NWindow*);

typedef int	 OSType;
typedef bool Bool;
typedef void UInt0;

typedef __UINT64_TYPE__ UInt64;
typedef __UINT32_TYPE__ UInt32;
typedef __UINT16_TYPE__ UInt16;
typedef __UINT8_TYPE__	UInt8;

typedef __SIZE_TYPE__ SizeT;

typedef __INT64_TYPE__ SInt64;
typedef __INT32_TYPE__ SInt32;
typedef __INT16_TYPE__ SInt16;
typedef __INT8_TYPE__  SInt8;

typedef char 			UTFChar;

/**
	@brief Standard library class.
*/
class NUser final
{
public:
	// THOSE REQUIRES PERMISSIONS FROM THE USER. //

	static UInt0 Poweroff();
	static UInt0 Reboot();
	static Bool	 IsWokeup();

	// THOSE DOESNT REQUIRE PERMISSIONS FROM THE USER. //

	static UInt0  Terminate();
	static Bool	  Exit(OSType code);
	static UInt0* New(long long sz);
	static UInt0  Delete(void* ptr);

	// ASK FOR ELEVATION //

	static Bool RaiseUAC();

	// THOSE MAY REQUIRE PERMISSIONS FROM THE USER. //

	static OSType Open(const char* path, const char* restr);
	static UInt0  Close(OSType descriptorType);
	static NURL*  Execute(const NURL* command);
	static UInt0* Read(const UTFChar* cmdNameOrData, SizeT cmdSize, OSType descriptorType);
	static UInt0* Write(const UTFChar* cmdNameOrData, SizeT cmdSize, OSType descriptorType);
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
	Bool			fAsyncOperationMode;
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

	virtual NWindow* Button(const char* text, NWindowCallback onClick = nullptr, NWindowCallback onDblClick = nullptr) = 0;

	virtual NWindow* Checkbox(const char* text, NWindowCallback onSelect = nullptr, NWindowCallback onUnselect = nullptr) = 0;

	virtual NWindow* Radio(const char* text, NWindowCallback onSelect = nullptr, NWindowCallback onUnselect = nullptr) = 0;

	virtual NWindow* Link(const char* where, const char* textIfAny = "", NWindowCallback onClick = nullptr, NWindowCallback onHover = nullptr) = 0;

public:
	Bool  fWindowEnabled;
	void* fWindowDataPtr;
};

/**
@brief This class contains an URL
*/
class NURL
{
public:
	explicit NURL() = default;
	virtual ~NURL() = default;

public:
	virtual NURL* Navigate(const char* url)		 = 0;
	virtual NURL* Protocol(const char* protocol) = 0;

public:
	char* fBufferPtr;
	long  fBufferLen;
};
