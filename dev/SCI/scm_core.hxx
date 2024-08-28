/* -------------------------------------------

Copyright ZKA Technologies.

File: rt.internal.inl
Purpose: Base code of SCM.

------------------------------------------- */

/// @internal

#ifndef __NDK__
#define object class
#define protocol class
#define clsid(X)

#warning ! You may be using the clang version of the ZKA kit, please be cautious that some thing mayn't be present. !
#endif // !__NDK__

// Interfaces are divided between classes.
// So that they aren't too big.

protocol IUnknown; // Refrenced from an IDB entry.
protocol UnknownUCLSID;	// From the IDB, the constructor of the object, e.g: TextUCLSID.
object UUID;

protocol clsid("d7c144b6-0792-44b8-b06b-02b227b547df") IUnknown
{
public:
	explicit IUnknown() = default;
	virtual ~IUnknown() = default;

	IUnknown& operator=(const IUnknown&) = default;
	IUnknown(const IUnknown&)			 = default;

	virtual SInt32 Release() = 0;
	virtual void RemoveRef() = 0;
	virtual IUnknown* AddRef() = 0;
	virtual VoidPtr QueryInterface(UUID* p_uuid) = 0;
};

/// @brief Allocate new SCM object.
/// @tparam TCLS the class type.
/// @tparam UCLSID UCLS factory class type.
/// @param uclsidOfCls UCLS factory class
/// @return TCLS interface
template <typename TCLS, typename UCLSID, typename... Args>
inline TCLS* ScmQueryInterface(UCLSID* uclsidOfCls, Args&&... args)
{
    uclsidOfCls->AddRef();
	return uclsidOfCls->QueryInterfaceWithArgs(args...);
}

/// @brief Release SCM object.
/// @tparam TCLS the class type.
/// @param cls the class to release.
/// @return status code.
template <typename TCLS>
inline SInt32 ScmReleaseClass(TCLS** cls)
{
	if (!cls)
		return -1;

	cls->RemoveRef();
	cls->Release();

	cls = nullptr;

	return 0;
}

template <typename FnSign, typename ClsID>
protocol IEventListener : public ClsID
{
public:
	explicit IEventListener() = default;
	virtual ~IEventListener() = default;

	IEventListener& operator=(const IEventListener&) = default;
	IEventListener(const IEventListener&)			 = default;

    virtual IEventListener& operator +=(FnSign arg)
	{
        this->AddEventListener(arg);
        return *this;
    }
};
