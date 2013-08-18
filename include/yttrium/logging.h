/// \file
/// \brief
/// \note Don't include this file in a common header!

#ifndef __Y_LOGGING_H
#define __Y_LOGGING_H

#include <yttrium/log.h>

#define Y_LOG_LEVEL_NORMAL 0 ///<
#define Y_LOG_LEVEL_DEBUG  1 ///<
#define Y_LOG_LEVEL_TRACE  2 ///<

/// \def Y_LOG_LEVEL
/// \brief

#ifndef Y_LOG_LEVEL
	#define Y_LOG_LEVEL Y_LOG_LEVEL_NORMAL
#endif

/// \def Y_LOG_DEBUG
/// \brief

/// \def Y_LOG_DEBUG_IF
/// \brief

#if Y_LOG_LEVEL >= Y_LOG_LEVEL_DEBUG
	#define Y_LOG_DEBUG(message) Y_LOG(message)
	#define Y_LOG_DEBUG_IF(condition, message) Y_LOG_IF(condition, message)
#else
	#define Y_LOG_DEBUG(message)
	#define Y_LOG_DEBUG_IF(condition, message)
#endif

/// \def Y_LOG_TRACE
/// \brief

/// \def Y_LOG_TRACE_IF
/// \brief

#if Y_LOG_LEVEL >= Y_LOG_LEVEL_TRACE
	#define Y_LOG_TRACE(message) Y_LOG(message)
	#define Y_LOG_TRACE_IF(condition, message) Y_LOG_IF(condition, message)
#else
	#define Y_LOG_TRACE(message)
	#define Y_LOG_TRACE_IF(condition, message)
#endif

#endif // __Y_LOGGING_H
