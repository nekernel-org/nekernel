#include <SCI/sci_base.hxx>

typedef int (*main_t)(int, char**);

int WinMain(int argc, char* argv[])
{
	if (argc < 3)
		return -1;

	auto mainDll = LdrOpenDLL(argv[1], argv[2]);

	int (*entrypointOff)(int, char**) = (main_t)LdrGetDLLProc(argv[3], mainDll);

	if (!entrypointOff)
		return -1;

	int ret = entrypointOff(argc, argv);

	LdrCloseDLL(mainDll);

	return ret;
}
