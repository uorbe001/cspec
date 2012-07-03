/*
 *  cspec_output_unit.c   :  Unit test like
 *
 * See copyright notice in cspec.h
 *
 */

#include <stdarg.h>
#include <stdio.h>
 #ifdef _WIN32
#	include <windows.h>
#endif
#include "cspec_output_unit.h"

typedef struct
{
	CSpecOutputStruct output;

	int nbPassed;
	int nbTotal;

} CSpecOutputUnitStruct;

typedef enum
{
	CSPEC_COLOR_RED = 1,
	CSPEC_COLOR_GREEN = 2,
	CSPEC_COLOR_YELLOW = 3
} CSpec_Color;

static CSpecOutputUnitStruct unit;


/* private functions */
static void coloredPrintf(CSpec_Color color, const char* format, ...);
#ifdef _WIN32
static WORD getWindowsColorAttribute(CSpec_Color color);
#else	/* !_WIN32 */
static int getAnsiColorCode(CSpec_Color color);
#endif	/* _WIN32 */

void startDescribeFunUnit( const char *descr)
{
	printf("Unit testing %s:\n\t", descr);
	unit.nbPassed = 0;
	unit.nbTotal  = 0;
}

void endDescribeFunUnit( )
{
	if(unit.nbPassed == unit.nbTotal)
	{
		coloredPrintf(CSPEC_COLOR_GREEN, "\n\tAll tests Passed (%d)\n\n", unit.nbTotal);
	}
	else
	{
		coloredPrintf(CSPEC_COLOR_RED, "\n\tPassed %d tests out of %d\n\n", unit.nbPassed, unit.nbTotal);
	}
}

void evalFunUnit(const char*filename, int line_number, const char*assertion, int assertionResult)
{
	unit.nbTotal++;
	if(assertionResult)
	{
		coloredPrintf(CSPEC_COLOR_GREEN, ".");
		unit.nbPassed++;
	}
	else
	{
		coloredPrintf(CSPEC_COLOR_RED, "\n\t:::Failed:::\t%s\n\t%s(line %d)\n", assertion, filename, line_number);
	}
}

void pendingFunUnit(const char* reason)
{
	coloredPrintf(CSPEC_COLOR_YELLOW, "\n\tPending: %s\n", reason);
}

CSpecOutputStruct* CSpec_NewOutputUnit()
{
	CSpec_InitOutput(& (unit.output) );
	
	unit.output.startDescribeFun	= startDescribeFunUnit;
	unit.output.endDescribeFun		= endDescribeFunUnit;
	unit.output.startItFun			= 0;
	unit.output.endItFun			= 0;
	unit.output.evalFun				= evalFunUnit;
	unit.output.pendingFun			= pendingFunUnit;

	return &(unit.output);
}

#ifdef _WIN32
static WORD
getWindowsColorAttribute(CSpec_Color color)
{
	WORD color_attribute;


	switch(color)
	{
	case CSPEC_COLOR_RED:
		color_attribute = FOREGROUND_RED;
		break;
	case CSPEC_COLOR_GREEN:
		color_attribute = FOREGROUND_GREEN;
		break;
	case CSPEC_COLOR_YELLOW:
		color_attribute = FOREGROUND_GREEN | FOREGROUND_RED;
		break;
	default:
		color_attribute = 0;
		break;
	}

	return color_attribute;
}
#else	/* !_WIN32 */
static int
getAnsiColorCode(CSpec_Color color)
{
	int color_code;


	switch(color)
	{
	case CSPEC_COLOR_RED:
		color_code = 31;
		break;
	case CSPEC_COLOR_GREEN:
		color_code = 32;
		break;
	case CSPEC_COLOR_YELLOW:
		color_code = 33;
		break;
	default:
		color_code = 30;
		break;
	}

	return color_code;
}
#endif	/* _WIN32 */

static void
coloredPrintf(CSpec_Color color, const char* format, ...)
{
#ifdef _WIN32
	HANDLE console_handle;
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	WORD default_color_attributes;
#endif	/* _WIN32 */
	va_list args;


	va_start(args, format);

#ifdef _WIN32

	console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console_handle, &buffer_info);
	default_color_attributes = buffer_info.wAttributes;

	/* Set color */
	SetConsoleTextAttribute(console_handle,
							getWindowsColorAttribute(color) |
							FOREGROUND_INTENSITY);

	/* Print Text */
	vprintf(format, args);

	/* Reset color */
	SetConsoleTextAttribute(console_handle,
							default_color_attributes);

#else	/* !_WIN32 */

	/* Set color */
	printf("\033[0;%dm", getAnsiColorCode(color));

	/* Print Text */
	vprintf(format, args);

	/* Reset color */
	printf("\033[m");

#endif	/* _WIN32 */

	va_end(args);
	return;
}