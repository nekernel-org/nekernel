/**
	Thu Oct 17 07:57:43 CEST 2024
	(c) Amlal EL Mahrouss.
*/

#ifndef APPS_STEPS_H
#define APPS_STEPS_H

#include <Common.h>
#include <Framework.h>

#define kStepsExtension ".stp"
#define kStepsStrLen (256U)

struct STEPS_COMMON_RECORD final
{
	SInt32 setup_magic;
	Char   setup_name[kStepsStrLen];
	Char   setup_company[kStepsStrLen];
	Char   setup_author[kStepsStrLen];
	SInt32 setup_version;
	SInt32 setup_pages;
	SInt32 setup_check_page, setup_eula_page;
};

#endif // ifndef APPS_STEPS_H