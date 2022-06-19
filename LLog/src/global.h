#ifndef GLOBAL_H
#define GLOBAL_H

#include "llog_global.h"

#ifdef C_OS_WIN
#include <windows.h>
#elif defined(C_OS_LINUX)
#include <unistd.h>
#include <fcntl.h>      // for func open()   
#include <sys/mman.h>   // for func mmap()
#include <sys/stat.h>
#include <sys/types.h>
#endif // C_OS_WIN

#include <assert.h>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <memory>
#include <mutex>
#include <thread>

#include <vector>

#include "tool.h"

#define LMAX(a, b)      a > b ? a : b
#define LMIN(a, b)      a < b ? a : b
#define SWAP(a, b)      { auto c = b; b = a; a = c; }

#define LOGLENTHMAX     1025

#define HOSTNAMELEN     256

#define FILEPATHLEN     1025
#define FILELEN         81

#define THREADMAX       16

#ifdef  ZERO
#undef  ZERO
#define ZERO    0
#else
#define ZERO    0
#endif // ZERO
   
#define LLINT64_LEN     18

LLOG_SPACE_BEGIN()
    typedef std::condition_variable         condition;
    typedef std::lock_guard<std::mutex>     mutex_guard;
    typedef std::unique_lock<std::mutex>    unique_mutex;
    typedef std::shared_ptr<std::thread>    shared_thread;

    const constexpr LLCHAR* llog_tags[6]
        = { "[DEBUG    ]", "[INFO     ]", "[WARNING  ]", "[ERROR    ]", "[CRITICAL ]", "[FATAL    ]" };
LLOG_SPACE_END()

#endif // !GLOBAL_H