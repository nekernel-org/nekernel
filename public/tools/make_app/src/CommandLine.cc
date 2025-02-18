/*
 * Created on Thu Oct 17 08:00:42 CEST 2024
 *
 * Copyright (c) 2024 Amlal EL Mahrouss
 */

#include <Framework.h>
#include <Steps.h>

#include <LibCF/Array.h>

/// @brief This program makes a framework directory for NeKernel OS.

int main(int argc, char* argv[])
{
	LibCF::CFArray<const char*, 255> files;

	auto ext = kFKExtension;

	for (SizeT i = 2UL; i < argc; ++i)
	{
		if (MmStrCmp(argv[i], "-h") == 0)
		{
			MsgAlloc(kAlertMsg);

			MsgSend(kAlertMsg, "%s", "make_app: Framework/Application Creation Tool.\n");
			MsgSend(kAlertMsg, "%s", "make_app: © Amlal EL Mahrouss, All rights reserved.\n");

			MsgSend(kAlertMsg, "%s", "make_app: -a: Application format.\n");
			MsgSend(kAlertMsg, "%s", "make_app: -s: Steps (Setup pages) format.\n");
			MsgSend(kAlertMsg, "%s", "make_app: -f: Framework format.\n");

			MsgShow(kAlertMsg);
			MsgFree(kAlertMsg);

			return EXIT_SUCCESS;
		}

		if (MmStrCmp(argv[i], "-a") == 0)
		{
			ext = kAppExtension;
			continue;
		}
		else if (MmStrCmp(argv[i], "-s") == 0)
		{
			ext = kStepsExtension;
			continue;
		}
		else if (MmStrCmp(argv[i], "-f") == 0)
		{
			ext = kFKExtension;
			continue;
		}

		files[i] = argv[i];
	}

	auto path = argv[1];

	if (FsCreateDir(path))
	{
		FsCreateDir(StrFmt("{}{}", path, kRootDirectory));
		FsCreateDir(StrFmt("{}{}", path, kExecDirectory));

		for (SInt32 i = 0; i < files.Count(); ++i)
		{
			auto& file = files[i];

			FsCopy(path, StrFmt("{}{}", path, file));
		}

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}
