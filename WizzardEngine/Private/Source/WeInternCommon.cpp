#include <stdio.h>
#include <stdarg.h>
#include "WeInternCommon.h"


WeErrorHandler::WeErrorHandler():
#ifdef __WEDEBUG__
mLevel(WeErrorLevel::L_INFO)
#else
mLevel(WeErrorLevel::L_ERROR)
#endif
{

}
WeErrorHandler::~WeErrorHandler()
{

}

void WeErrorHandler::setErrorLevel(WeErrorLevel::Enum level)
{
	mLevel = level;
}
void WeErrorHandler::errorHandle(WeErrorCode::Enum code, const char* file, int line, const char* fmt, ...)
{
	if (mLevel >= WeErrorLevel::L_ERROR){
		printf("Error %d in file %s, line %d: ", code, file, line);
		va_list args;
		va_start(args, fmt);
		printf_we(fmt, args);
		va_end(args);
	}
		
}

void WeErrorHandler::debugHandle(WeErrorCode::Enum code, const char* file, int line, const char* fmt, ...)
{
	if (mLevel >= WeErrorLevel::L_WARNNING){
		printf("Debug %d in file %s, line %d: ", code, file, line);
		va_list args;
		va_start(args, fmt);
		printf_we(fmt, args);
		va_end(args);
	}
		
}

void WeErrorHandler::infoHandle(WeErrorCode::Enum code, const char* file, int line, const char* fmt, ...)
{
	if (mLevel >= WeErrorLevel::L_INFO)
	{
		printf("Info %d in file %s, line %d: ", code, file, line);
		va_list args;
		va_start(args, fmt);
		printf_we(fmt, args);
		va_end(args);
	}
		
}


void WeErrorHandler::printf_we(const char* fmt, va_list& args)
{
	if (vsnprintf(mBuffer, BUFFER_SIZE - 1, fmt, args) <= 0)
	{
		mBuffer[BUFFER_SIZE - 1] = '\0';
	}

	printf(mBuffer);
	printf("\n");
	fflush(stdout);
}

class WeErrorHandler gErrorHandler;
class WeErrorHandler& weGetErrorHandler()
{
	return gErrorHandler;
}