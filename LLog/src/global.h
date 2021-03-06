#ifndef GLOBAL_H
#define GLOBAL_H

#include "llog_global.h"

#ifdef C_OS_WIN
#include <windows.h>
#elif defined(C_OS_LINUX)
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif // C_OS_WIN
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <memory>
#include <mutex>
#include <thread>

#define LMAX(a, b)      a > b ? a : b
#define LMIN(a, b)      a < b ? a : b
#define SWAP(a, b)      { auto c = b; b = a; a = c; }
#define HOSTNAMELEN     256
#define FILEPATHLEN     1025
#define FILELEN         81
#define THREADMAX       8

#ifdef  ZERO
#undef  ZERO
#define ZERO    0
#else
#define ZERO    0
#endif // ZERO


namespace LLog {

    const constexpr LUINT32 buffer_size = 1024 * 1024 * 4;

    const constexpr LLCHAR* llog_tags[6]
        = { "DEBUG    ", "INFO     ", "WARNING  ", "ERROR    ", "CRITICAL ", "FATAL    " };

    typedef struct LSTRING {
        LUINT32 _size = 0;
        LLCHAR* _str = nullptr;

        explicit LSTRING(LLCHAR * str = nullptr) : _str(str){}

    } LSTRING;

    template < typename T, typename Tuple >
    struct DataType;

    template < typename T, typename ... Types >
    struct DataType < T, std::tuple < T, Types... > > {
        static constexpr const LUINT32 value = 0;
    };

    template < typename T, typename U, typename ... Types >
    struct DataType < T, std::tuple < U, Types... > > {
        static constexpr const LUINT32 value = 1 + DataType < T, std::tuple < Types... > >::value;
    };

    typedef std::tuple < LLCHAR, LLCHAR*, LLINT16, LLINT32, LLINT64, LUINT16, LUINT32, LUINT64, LLFLOAT, LLDOUBLE, LSTRING > SupportedTypes;
}

#endif // !GLOBAL_H