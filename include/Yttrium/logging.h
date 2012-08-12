/// \file
/// \brief
/// \note Don't include this file in a common header!

#ifndef __Y_LOGGING_H
#define __Y_LOGGING_H

#include <Yttrium/log.h>

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

#if Y_LOG_LEVEL >= Y_LOG_LEVEL_DEBUG
	#define Y_LOG_TRACE(message) Y_LOG(message)
#else
	#define Y_LOG_TRACE(message)
#endif

/// \def Y_LOG_TRACE
/// \brief

#if Y_LOG_LEVEL >= Y_LOG_LEVEL_TRACE
	#define Y_LOG_TRACE(message) Y_LOG(message)
#else
	#define Y_LOG_TRACE(message)
#endif

#endif // __Y_LOGGING_H
