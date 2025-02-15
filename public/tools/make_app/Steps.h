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
	char	setup_name[255];
	char	setup_company[255];
	char	setup_author[255];
	int32_t setup_version;
	int32_t setup_pages;
};

#endif // ifndef APPS_STEPS_H