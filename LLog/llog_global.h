#ifndef LLOG_GLOBAL_H
#define LLOG_GLOBAL_H

/**
 * @brief OS check
 *
 */
#if !defined(SAG_COM) && (!defined(WINAPI_FAMILY) || WINAPI_FAMILY==WINAPI_FAMILY_DESKTOP_APP) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define C_OS_WIN32
#  define C_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  if defined(WINAPI_FAMILY)
#    ifndef WINAPI_FAMILY_PC_APP
#      define WINAPI_FAMILY_PC_APP WINAPI_FAMILY_APP
#    endif
#    if defined(WINAPI_FAMILY_PHONE_APP) && WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#      define C_OS_WINRT
#    elif WINAPI_FAMILY == WINAPI_FAMILY_PC_APP
#      define C_OS_WINRT
#    else
#      define C_OS_WIN32
#    endif
#  else
#    define C_OS_WIN32
#  endif
#elif defined(__linux__) || defined(__linux)
#  define C_OS_LINUX
#else
#  error "not support this OS"
#endif

#if defined(C_OS_WIN32) || defined(C_OS_WIN64) || defined(C_OS_WINRT)
#  define C_OS_WIN
#endif

#if defined(C_OS_WIN)
#  undef C_OS_UNIX
#elif !defined(C_OS_UNIX)
#  define C_OS_UNIX
#endif

#ifdef C_OS_WIN
typedef bool                LLBOOL;
typedef char                LLCHAR;
typedef signed char         LLINT8;
typedef signed short        LLINT16;
typedef signed int          LLINT32;
typedef signed long long    LLINT64;
typedef unsigned char       LUINT8;
typedef unsigned short      LUINT16;
typedef unsigned int        LUINT32;
typedef unsigned long long  LUINT64;
typedef float               LLFLOAT;
typedef double              LLDOUBLE;
#elif defined(C_OS_LINUX)
typedef bool                LLBOOL;
typedef char                LLCHAR;
typedef signed char         LLINT8;
typedef signed short        LLINT16;
typedef signed int          LLINT32;
typedef signed long int     LLINT64;
typedef unsigned char       LUINT8;
typedef unsigned short      LUINT16;
typedef unsigned int        LUINT32;
typedef unsigned long int   LUINT64;
typedef float               LLFLOAT;
typedef double              LLDOUBLE;
#endif // C_OS_WIN

#define SPACE_BEGIN(NAME)   namespace NAME {
#define SPACE_END()         }

#define USING_LLOG
#ifndef USING_LLOG
#define LLOG_SPACE_BEGIN()
#define LLOG_SPACE_END()
#else
#define LLOG_SPACE_BEGIN()  SPACE_BEGIN(LLog)
#define LLOG_SPACE_END()    SPACE_END()
#endif

LLOG_SPACE_BEGIN()
    /**
     * @brief The level of LLog.
     */
    enum Level {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        CRITICAL = 4,
        FATAL = 5
    };
LLOG_SPACE_END()


#endif // !LLOG_GLOBAL_H