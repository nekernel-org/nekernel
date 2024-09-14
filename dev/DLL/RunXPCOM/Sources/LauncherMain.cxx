#include <SCI/sci_base.hxx>

typedef SInt32 (*RPCProcKind)(IUnknown*);

/**
 * @brief Main procedure for DLL runner.
 */
int WinMain(int argc, char* argv[])
{
	if (argc < 4)
		return -1;

	auto mainDll = LdrOpenDLL(argv[1], argv[2]);

	IUnknown*	interface = (IUnknown*)LdrGetDLLProc(argv[3], mainDll);
	RPCProcKind dllRpc	  = (RPCProcKind)LdrGetDLLProc(argv[4], mainDll);

	if (!dllRpc)
		return -1;
	if (!interface)
		return -1;

	auto ret = dllRpc(interface);

	LdrCloseDLL(mainDll);

	return ret;
}
