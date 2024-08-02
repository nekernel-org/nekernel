/* -------------------------------------------

Copyright ZKA Technologies.

File: rt.internal.inl
Purpose: Base code of SCM.

------------------------------------------- */

/// @internal

#ifndef __ZECC__
#define object class
#define protocol class
#define clsid(X) __attribute__((uuid(X)))

#warning ! you may be using the clang version of the newos kit, please be cautious that some thing mayn't be present. !
#endif // !__ZECC__

// Interfaces are divided between classes.
// So that they aren't too big.

protocol UnknownInterface; // Refrenced from an IDB entry.
class UnknownUCLSID;	// From the IDB, the constructor of the object, e.g: TextUCLSID.
object UUID;

protocol clsid("d7c144b6-0792-44b8-b06b-02b227b547df") UnknownInterface
{
public:
	explicit UnknownInterface() = default;
	virtual ~UnknownInterface() = default;

	UnknownInterface& operator=(const UnknownInterface&) = default;
	UnknownInterface(const UnknownInterface&)			 = default;
 
	virtual SInt32 Release() = 0;
	virtual void DecrementRef() = 0;
	virtual UnknownInterface* IncrementRef() = 0;
	virtual VoidPtr QueryInterface(UUID* p_uuid) = 0;
};

/// @brief Allocate new SCM object.
/// @tparam TCLS 
/// @tparam UCLSID 
/// @param uclsidOfCls 
/// @return 
template <typename TCLS, typename UCLSID, typename... Args>
inline TCLS* ScmQueryInterface(UCLSID* uclsidOfCls, Args&&... args)
{
	return uclsidOfCls->QueryInterfaceWithArgs(args...);
}

/// @brief Release SCM object.
/// @tparam TCLS 
/// @param cls 
/// @return 
template <typename TCLS>
inline SInt32 ScmReleaseClass(TCLS* cls)
{
	if (!cls)
		return -1;

	cls->DecrementRef();
	cls->Release();

	return 0;
}
