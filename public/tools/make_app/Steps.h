/**
	Thu Oct 17 07:57:43 CEST 2024
	(c) Amlal El Mahrouss.
*/

#ifndef APPS_STEPS_H
#define APPS_STEPS_H

#include <Common.h>
#include <Framework.h>

#define kStepsExtension ".stp"
#define kStepsStrLen	(256U)

#define kStepsMagic	   " pls"
#define kStepsMagicLen (4U)
#define kStepsVersion  (0x0100)

#define kStepsMime "ne-application-kind/steps"

struct STEPS_COMMON_RECORD final
{
	Char   magic[kStepsMagicLen];
	Char   name[kStepsStrLen];
	Char   company[kStepsStrLen];
	Char   author[kStepsStrLen];
	SInt32 version;
	SInt32 pages;
	SInt32 check_page, eula_page;
};

#endif // ifndef APPS_STEPS_H