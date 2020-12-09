#pragma once

// TODO: more robust logging, flags, etc

#ifndef NDEBUG
#define FUI_DEBUG
#endif

#ifdef FUI_DEBUG
	#ifdef _WIN32
		#define FUI_BREAK __debugbreak()
	#else
		#define FUI_BREAK do { } while (0)
	#endif
#else
	#define FUI_BREAK do { } while (0)
#endif

void fui_log(const char* severity, const char* format, ...);

#ifdef FUI_DEBUG
	#define FUI_LOG(severity, x, ...) do { fui_log(severity, x, __VA_ARGS__); } while(0)
#else
	#define FUI_LOG(severity, x, ...) do { } while(0)
#endif

#define FUI_LOG_DEBUG(x, ...) FUI_LOG("DEBUG", x, __VA_ARGS__)
#define FUI_LOG_WARN(x, ...) FUI_LOG("WARN", x, __VA_ARGS__)
#define FUI_LOG_ERROR(x, ...) FUI_LOG("ERROR", x, __VA_ARGS__)
#define FUI_LOG_FATAL(x, ...) FUI_LOG("FATAL", x, __VA_ARGS__)

#define FUI_ASSERT_MSG(condition, msg)                \
do {                                                  \
	if(!(condition)){                                 \
		FUI_LOG_FATAL("---------------------","");    \
		FUI_LOG_FATAL("ASSERTION FAILED!","");        \
		FUI_LOG_FATAL("File: %s", __FILE__);          \
		FUI_LOG_FATAL("Line: %d", __LINE__);          \
		FUI_LOG_FATAL("Function: %s", __FUNCTION__);  \
		FUI_LOG_FATAL("Condition: (%s)", #condition); \
		FUI_LOG_FATAL("Details: %s", #msg);           \
		FUI_LOG_FATAL("---------------------","");    \
		FUI_BREAK;                                    \
	}                                                 \
} while(0)

#define FUI_ASSERT(condition) FUI_ASSERT_MSG(condition, "-")

#ifdef FUI_DEBUG
	#define FUI_DEBUG_ASSERT(condition) FUI_ASSERT(condition)
	#define FUI_DEBUG_ASSERT_MSG(condition, msg) FUI_ASSERT_MSG(condition, msg)
#else
	#define FUI_DEBUG_ASSERT(condition) do { } while(0)
	#define FUI_DEBUG_ASSERT_MSG(condition, msg) do { } while(0)
#endif
