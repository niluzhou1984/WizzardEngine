
#ifndef WE_INTERN_COMMON_H
#define WE_INTERN_COMMON_H

#include <stdarg.h>
#include <assert.h>

struct WeErrorLevel
{
	enum Enum
	{
		L_NONE = -1,
		L_ERROR = 0,
		L_WARNNING = 1,
		L_INFO = 2,
	};

};


struct WeErrorCode
{
	enum Enum
	{
		ERROR_START = 0,
		ERROR_BSP,
		ERROR_SCENE_INIT,

		WARN_START = 0x100,
		WARN_CREATE_SHAPE,
		WARN_CONVEX_COOKING,
		WARN_QUERY,

		INFO_START = 0x200,
		INFO_DEBUG,

	};
};


struct WeStatusCode
{
	enum Enum
	{
		STATUS_OK = 0,
		STATUS_ERROR,
	};
};



#define WE_ASSERT(expr) assert(expr)

#define WE_ERROR_HANDLE(code, fmt, ...) \
	weGetErrorHandler().errorHandle(code, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define WE_DEBUG_HANDLE(code, fmt, ...) \
	weGetErrorHandler().debugHandle(code, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define WE_INFO_HANDLE(code, fmt, ...) \
	weGetErrorHandler().infoHandle(code, __FILE__, __LINE__, fmt, ##__VA_ARGS__)


class WeErrorHandler
{
public:
	WeErrorHandler();
	~WeErrorHandler();

	void setErrorLevel(WeErrorLevel::Enum level);
	void errorHandle(WeErrorCode::Enum code, const char* file, int line, const char* fmt, ...);
	void debugHandle(WeErrorCode::Enum code, const char* file, int line, const char* fmt, ...);
	void infoHandle(WeErrorCode::Enum code, const char* file, int line, const char* fmt, ...);

private:
	const static int BUFFER_SIZE = 1024;
	void printf_we(const char* fmt, va_list& args);
	WeErrorLevel::Enum	mLevel;
	char				mBuffer[BUFFER_SIZE];
};

class WeErrorHandler& weGetErrorHandler();

#endif