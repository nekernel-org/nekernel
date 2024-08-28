#include <SCI/sci_base.hxx>

typedef int (*rpc_call_t)(IUnknown*);

int WinMain(int argc, char* argv[])
{
	if (argc < 4)
		return -1;

	auto mainDll = LdrOpenDLL(argv[1], argv[2]);

	IUnknown*  interface = (IUnknown*)LdrGetDLLProc(argv[3], mainDll);
	rpc_call_t dllRpc	 = (rpc_call_t)LdrGetDLLProc(argv[4], mainDll);

	if (!dllRpc)
		return -1;
	if (!interface)
		return -1;

	auto ret = dllRpc(interface);

	LdrCloseDLL(mainDll);

	return ret;
}
