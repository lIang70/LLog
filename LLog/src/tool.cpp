#include "tool.h"

void 
Tool::format_timestamp(LLCHAR* _buffer, LUINT64 _timestamp) {
    // format: %4Y-%2M-%2D %2H:%2M:%2S.%6ms
    std::time_t time_t = _timestamp / 1000000;
#ifdef C_OS_WIN
    tm gmtime;
    gmtime_s(&gmtime, &time_t);
    _buffer += Tool::num2string((LLINT64)gmtime.tm_year + 1900, _buffer, 4);
    *_buffer++ = '-';
    _buffer += Tool::num2string((LLINT64)gmtime.tm_mon + 1, _buffer, 2);
    *_buffer++ = '-';
    _buffer += Tool::num2string((LLINT64)gmtime.tm_mday, _buffer, 2);
    *_buffer++ = ' ';
    _buffer += Tool::num2string((LLINT64)gmtime.tm_hour, _buffer, 2);
    *_buffer++ = ':';
    _buffer += Tool::num2string((LLINT64)gmtime.tm_min, _buffer, 2);
    *_buffer++ = ':';
    _buffer += Tool::num2string((LLINT64)gmtime.tm_sec, _buffer, 2);
    *_buffer++ = '.';
    _buffer += Tool::num2string(_timestamp % 1000000, _buffer, 6);
#elif defined(C_OS_LINUX)
    tm gmtime = *std::gmtime(&time_t);
    _buffer += Tool::num2string((LLINT64)gmtime.tm_year + 1900, _buffer, 4);
    *_buffer++ = '-';
    _buffer += Tool::num2string((LLINT64)gmtime.tm_mon + 1, _buffer, 2);
    *_buffer++ = '-';
    _buffer += Tool::num2string((LLINT64)gmtime.tm_mday, _buffer, 2);
    *_buffer++ = ' ';
    _buffer += Tool::num2string((LLINT64)gmtime.tm_hour, _buffer, 2);
    *_buffer++ = ':';
    _buffer += Tool::num2string((LLINT64)gmtime.tm_min, _buffer, 2);
    *_buffer++ = ':';
    _buffer += Tool::num2string((LLINT64)gmtime.tm_sec, _buffer, 2);
    *_buffer++ = '.';
    _buffer += Tool::num2string(_timestamp % 1000000, _buffer, 6);
#endif // C_OS_WIN
}

static constexpr char digit_pairs[201] = {
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

LUINT32 
Tool::num2string(LLINT64 _num, LLCHAR* _buffer, LUINT32 _min_size) {
    LUINT32 _cap = std::numeric_limits<LLINT64>::digits10 + 3, _size = 0;
    char* p = _buffer + _cap;
    if (p == nullptr)
        return (ZERO);
    
    const bool isNegative = _num < 0;
    _num = isNegative ? -_num : _num;

    while (_num > 99) {
        LUINT32 pos = _num % 100;
        _num /= 100;
        p -= 2;
        _size += 2;
        *(LLINT16*)(p) = *(LLINT16*)(digit_pairs + 2 * pos);
    }

    if (_num > 9) {
        p -= 2;
        _size += 2;
        *(LLINT16*)(p) = *(LLINT16*)(digit_pairs + 2 * _num);
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
    LUINT32 _cap = std::numeric_limits<LLINT64>::digits10 + 2, _size = 0;
    char* p = _buffer + _cap;
    if (p == nullptr)
        return (ZERO);
    
    while (_num > 99) {
        LUINT32 pos = _num % 100;
        _num /= 100;
        p -= 2;
        _size += 2;
        *(LLINT16*)(p) = *(LLINT16*)(digit_pairs + 2 * pos);
    }

    if (_num > 9) {
        p -= 2;
        _size += 2;
        *(LLINT16*)(p) = *(LLINT16*)(digit_pairs + 2 * _num);
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
