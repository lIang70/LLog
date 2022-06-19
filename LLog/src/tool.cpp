#include "global.h"
#include "tool.h"

static constexpr char digit_pairs_2[201] = {
    "00010203040506070809"
    "10111213141516171819"
    "20212223242526272829"
    "30313233343536373839"
    "40414243444546474849"
    "50515253545556575859"
    "60616263646566676869"
    "70717273747576777879"
    "80818283848586878889"
    "90919293949596979899"
};

#ifdef C_OS_WIN
static struct tm    gmtime;
#endif // C_OS_WIN
static LLCHAR       __host_name[HOSTNAMELEN];
static LLINT32      __process_id = 0;

LLBOOL
Tool::check_binary(LLINT64 _num) {
    return (_num & (_num - 1)) == 0;
}

LLINT64
Tool::get_minimum_greater(LLINT64 _num) {
    if (check_binary(_num)) return _num;
    LLINT32 _level = 0;
    while (_num != 1) {
        _num /= 2;
        _level++;
    }
    return (1<<(_level+1));
}

LUINT64 
Tool::get_system_time() {
    // register LUINT32 lo, hi;
    // __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
    // return (((LUINT64)hi << 32) | lo);
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

LUINT32 
Tool::get_tid() {
#ifdef C_OS_WIN
    static thread_local const LUINT32 id = GetCurrentThreadId();
#elif defined(C_OS_LINUX)
    static thread_local const LUINT32 id = gettid();
#endif // C_OS_WIN
    return id;
}

void
Tool::init_system_info() {
    memset(__host_name, 0, sizeof(__host_name));
#ifdef C_OS_WIN
    __process_id = GetCurrentProcessId();
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerName(__host_name, &size);
#elif defined(C_OS_LINUX)
    __process_id = getpid();
    gethostname(__host_name, sizeof(__host_name));
#endif // C_OS_WIN
}

const LUINT32 
Tool::get_pid() {
    return __process_id;
}

const LLCHAR* 
Tool::get_hostname() {
    return __host_name;
}

void 
Tool::format_timestamp(LLCHAR* _buffer, LUINT64 _timestamp) {
    std::time_t _time_t = _timestamp / 1000000;
    std::time_t _ns = _timestamp - 1000000 * _time_t;
#ifdef C_OS_WIN
    gmtime_s(&gmtime, &_time_t);
#elif defined(C_OS_LINUX)
    struct tm* gmtime = std::gmtime(&_time_t);
#endif // C_OS_WIN
    LUINT32 pos1, pos2, pos3;
    pos1 = (gmtime->tm_year + 1900) / 100; 
    pos2 = (gmtime->tm_year + 1900) - pos1 * 100;
    *(LLINT16*)(_buffer) = *(LLINT16*)(digit_pairs_2 + 2 * pos1); _buffer += 2;
    *(LLINT16*)(_buffer) = *(LLINT16*)(digit_pairs_2 + 2 * pos2); _buffer += 2;
    *_buffer++ = '-';
    *(LLINT16*)(_buffer) = *(LLINT16*)(digit_pairs_2 + 2 * (gmtime->tm_mon + 1)); _buffer += 2;
    *_buffer++ = '-';
    *(LLINT16*)(_buffer) = *(LLINT16*)(digit_pairs_2 + 2 * (gmtime->tm_mday)); _buffer += 2;
    *_buffer++ = ' ';
    *(LLINT16*)(_buffer) = *(LLINT16*)(digit_pairs_2 + 2 * (gmtime->tm_hour)); _buffer += 2;
    *_buffer++ = ':';
    *(LLINT16*)(_buffer) = *(LLINT16*)(digit_pairs_2 + 2 * (gmtime->tm_min)); _buffer += 2;
    *_buffer++ = ':';
    *(LLINT16*)(_buffer) = *(LLINT16*)(digit_pairs_2 + 2 * (gmtime->tm_sec)); _buffer += 2;
    *_buffer++ = '.';
    pos1 = _ns / 10000;
    pos2 = (_ns - pos1 * 10000) / 100;
    pos3 = _ns - pos1 * 10000 - pos2 * 100;
    *(LLINT16*)(_buffer) = *(LLINT16*)(digit_pairs_2 + 2 * pos1); _buffer += 2;
    *(LLINT16*)(_buffer) = *(LLINT16*)(digit_pairs_2 + 2 * pos2); _buffer += 2;
    *(LLINT16*)(_buffer) = *(LLINT16*)(digit_pairs_2 + 2 * pos3);
}

LUINT32 
Tool::num2string(LLINT64 _num, LLCHAR* _buffer, LUINT32 _min_size) {
    LUINT32 _cap = LLINT64_LEN + 3, _size = 0;
    char* p = _buffer + _cap;
    if (p == nullptr)
        return (ZERO);
    
    const bool isNegative = _num < 0;
    _num = isNegative ? -_num : _num;

    while (_num > 99) {
        LUINT64 last = _num;
        _num /= 100;
        LUINT32 pos = last - _num * 100;
        p -= 2;
        _size += 2;
        *(LLINT16*)(p) = *(LLINT16*)(digit_pairs_2 + 2 * pos);
    }

    if (_num > 9) {
        p -= 2;
        _size += 2;
        *(LLINT16*)(p) = *(LLINT16*)(digit_pairs_2 + 2 * _num);
    } else {
        *--p = '0' + char(_num);
        _size++;
    }

    _cap = _size;
    if (isNegative) {
        *--p = '-';
        _size++;
        _cap++;
    } else {
        if (_min_size > _size) {
            _min_size = _min_size - _size;
            _size += _min_size;
            while (_min_size--)
                *_buffer++ = '0';
        }
    }
    
    while (_cap--)
        *_buffer++ = *p++;

    return _size;
}

LUINT32 
Tool::num2string(LUINT64 _num, LLCHAR* _buffer, LUINT32 _min_size) {
    LUINT32 _cap = LLINT64_LEN + 2, _size = 0;
    char* p = _buffer + _cap;
    if (p == nullptr)
        return (ZERO);
    
    while (_num > 99) {
        LUINT64 last = _num;
        _num /= 100;
        LUINT32 pos = last - _num * 100;
        p -= 2;
        _size += 2;
        *(LLINT16*)(p) = *(LLINT16*)(digit_pairs_2 + 2 * pos);
    }

    if (_num > 9) {
        p -= 2;
        _size += 2;
        *(LLINT16*)(p) = *(LLINT16*)(digit_pairs_2 + 2 * _num);
    } else {
        *--p = '0' + char(_num);
        _size++;
    }

    _cap = _size;
    if (_min_size > _size) {
        _min_size = _min_size - _size;
        _size += _min_size;
        while (_min_size--)
            *_buffer++ = '0';
    }

    while (_cap--)
        *_buffer++ = *p++;

    return _size;
}
