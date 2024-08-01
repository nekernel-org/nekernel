/* -------------------------------------------

Copyright ZKA Technologies.

File: rt.internal.inl.
Purpose: Internal file for SCM.

------------------------------------------- */


// Interfaces are divided between classes.
// So that they aren't too big.

class UnknownInterface; // Refrenced from an IDB entry.
class UnknownUCLSID;	// From the IDB, the constructor of the object, e.g: TextUCLSID.
class UUID;

class __attribute__((uuid("d7c144b6-0792-44b8-b06b-02b227b547df"))) UnknownInterface
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

/// @brief Allocate new SCM class.
/// @tparam TCLS 
/// @tparam UCLSID 
/// @param uclsidOfCls 
/// @return 
template <typename TCLS, typename UCLSID, typename... Args>
inline TCLS* RtlQueryInterface(UCLSID* uclsidOfCls, Args&&... args)
{
	return uclsidOfCls->QueryInterfaceWithArgs(args...);
}

/// @brief Release SCM class.
/// @tparam TCLS 
/// @param cls 
/// @return 
template <typename TCLS>
inline SInt32 RtlReleaseClass(TCLS* cls)
{
	if (!cls)
		return -1;

	cls->DecrementRef();
	cls->Release();

	return 0;
}
