/* -------------------------------------------

Copyright ZKA Technologies.

File: rt.internal.inl.
Purpose: Internal file for SCM.

------------------------------------------- */

/// @brief Allocate new SCM class.
/// @tparam TCLS 
/// @tparam UCLSID 
/// @param uclsidOfCls 
/// @return 
template <typename TCLS, typename UCLSID, typename... Args>
inline TCLS* RtlGetClassFromCLSID(UCLSID* uclsidOfCls, Args&&... args)
{
	return uclsidOfCls->QueryObjectWithArgs(args...);
}

/// @brief Release SCM class.
/// @tparam TCLS 
/// @param cls 
/// @return 
template <typename TCLS>
inline SInt32 RtlReleaseClass(TCLS* cls)
{
	cls->DecrementRef();
	cls->Release();

	return 0;
}
