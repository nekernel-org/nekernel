/**
	Thu Oct 17 07:57:43 CEST 2024
	(c) Amlal EL Mahrouss.
*/

#ifndef APPS_STEPS_H
#define APPS_STEPS_H

#include <Common.h>
#include <Framework.h>

#define kStepsExtension ".stp"

struct StepsCommonRecord final
{
	Char   setup_name[256U];
	Char   setup_company[256U];
	Char   setup_author[256U];
	SInt32 setup_version;
	SInt32 setup_pages;
};

#endif // ifndef APPS_STEPS_H