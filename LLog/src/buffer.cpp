#include "buffer.h"

LLog::__buffer::__buffer(LUINT32 _size)
    : m_nBufferCap(m_nFixedSize), m_nBufferI(0) {
    is_needed_resize(_size < m_nFixedSize ? 0 : _size - m_nFixedSize);
}

LLog::__buffer::__buffer(const __buffer& _another) noexcept
    : m_nBufferCap(m_nFixedSize), m_nBufferI(0) {
    if (!is_needed_resize(_another.m_nBufferI)) {
        memcpy(m_cStackBuffer, _another.m_cStackBuffer, _another.m_nBufferI);
    } else {
        memcpy(m_uHeapBuffer.get(), _another.m_uHeapBuffer.get(), _another.m_nBufferI);
    }
    m_nBufferI = _another.m_nBufferI;
}

LLog::__buffer::__buffer(__buffer&& _another) noexcept
    : m_nBufferCap(m_nFixedSize), m_nBufferI(0) {
    if (!is_needed_resize(_another.m_nBufferI)) {
        memcpy(m_cStackBuffer, _another.m_cStackBuffer, _another.m_nBufferI);
    } else {
        m_uHeapBuffer.swap(_another.m_uHeapBuffer);
    }
    m_nBufferI = _another.m_nBufferI;
}

LLog::__buffer&
LLog::__buffer::operator=(const __buffer& _another) noexcept {
    m_nBufferI = 0;
    if (!is_needed_resize(_another.m_nBufferI) && !m_uHeapBuffer && !_another.m_uHeapBuffer) {
        memcpy(m_cStackBuffer, _another.m_cStackBuffer, _another.m_nBufferI);
    }
    else if (_another.m_uHeapBuffer) {
        memcpy(m_uHeapBuffer.get(), _another.m_uHeapBuffer.get(), _another.m_nBufferI);
    }
    else {
        memcpy(m_uHeapBuffer.get(), _another.m_cStackBuffer, _another.m_nBufferI);
    }
    m_nBufferI = _another.m_nBufferI;
    return *this;
}

LLog::__buffer&
LLog::__buffer::operator=(__buffer&& _another) noexcept {
    m_nBufferI = 0;
    if (!is_needed_resize(_another.m_nBufferI) && !m_uHeapBuffer && !_another.m_uHeapBuffer) {
        memcpy(m_cStackBuffer, _another.m_cStackBuffer, _another.m_nBufferI);
    }
    else if (_another.m_uHeapBuffer) {
        m_uHeapBuffer.swap(_another.m_uHeapBuffer);
    }
    else {
        memcpy(m_uHeapBuffer.get(), _another.m_cStackBuffer, _another.m_nBufferI);
    }
    m_nBufferI = _another.m_nBufferI;

    return *this;
}

LUINT32
LLog::__buffer::index_add(LUINT32 _size) {
    m_nBufferI += _size;
    return m_nBufferI - _size;
}

LLCHAR*
LLog::__buffer::buffer_begin() {
    return !m_uHeapBuffer ? m_cStackBuffer : m_uHeapBuffer.get();
}

LUINT32
LLog::__buffer::cap() const {
    return m_nBufferCap;
}

LLBOOL
LLog::__buffer::is_needed_resize(LUINT32 _size) {
    LUINT32 const _RequiredSize = m_nBufferI + _size;

    if (_RequiredSize < m_nBufferCap)
        return false;

    if (!m_uHeapBuffer) {
        m_nBufferCap = LMAX(static_cast<LUINT32>(512), _RequiredSize);
        m_uHeapBuffer.reset(new char[m_nBufferCap]);
        memcpy(m_uHeapBuffer.get(), m_cStackBuffer, m_nBufferI);
    }
    else {
        m_nBufferCap = LMAX(2 * m_nBufferCap, _RequiredSize);
        std::unique_ptr < char[] > _NewHeapBuffer(new char[m_nBufferCap]);
        memcpy(_NewHeapBuffer.get(), m_uHeapBuffer.get(), m_nBufferI);
        m_uHeapBuffer.swap(_NewHeapBuffer);
    }
    return true;
}

void 
LLog::__buffer::swap(__buffer& _buffer) {
    if (m_uHeapBuffer || _buffer.m_uHeapBuffer) {
        m_uHeapBuffer.swap(_buffer.m_uHeapBuffer);
    }

    if (!m_uHeapBuffer || !_buffer.m_uHeapBuffer) {
        LLCHAR _tempBuffer[m_nFixedSize];
        memcpy(_tempBuffer, _buffer.m_cStackBuffer, m_nFixedSize);
        memcpy(_buffer.m_cStackBuffer, m_cStackBuffer, m_nFixedSize);
        memcpy(m_cStackBuffer, _tempBuffer, m_nFixedSize);
    }
    
    SWAP(m_nBufferI, _buffer.m_nBufferI)
    SWAP(m_nBufferCap, _buffer.m_nBufferCap)
}

#define NLLCHAR2BUFFER(__buffer, __index, nllchar, n)   \
    memcpy(__buffer + __index, nllchar, n); __index += n;
#define DECODESTRING(__buffer, __index, str) {  \
    auto _str = str;                            \
    while (*_str != '\0') {                     \
        *(__buffer + __index++) = *_str++;      \
    }                                           \
}
#define CHECKMEM(_src, _index, _size, _need_size) {                     \
    if (_index + _need_size > _size) {                                  \
        auto _tmp = (LLCHAR*)realloc(_buffer,                           \
            LMAX(_size * 2, _index + _need_size));                      \
        if (_tmp != nullptr) {                                          \
            _buffer = _tmp;                                             \
            _size = LMAX(_size * 2, _index + _need_size);               \
        } else                                                          \
            printf("[LLOG] Fatal : LLog::Buffer 'realloc' failure!\n"); \
    }                                                                   \
}                                                   

void 
LLog::Stream::stringify(LLCHAR*& _buffer, LUINT32& _index, LUINT32 _size, LLCHAR* _start, const LLCHAR* const _end) {
    LUINT32 type_id; 
    while (_start != _end) {
        type_id = static_cast<LUINT32>(*_start); _start++;
        switch (type_id) {
        case DataType<LLCHAR, LLog::SupportedTypes >::value:
            CHECKMEM(_buffer, _index, _size, sizeof(LLCHAR))
            _buffer[_index++] = *reinterpret_cast<LLCHAR*>(_start); _start++;
            break;
        case DataType<LLCHAR*, LLog::SupportedTypes >::value:
            type_id = *reinterpret_cast<LUINT32*>(_start); _start += sizeof(LUINT32);
            CHECKMEM(_buffer, _index, _size, type_id * sizeof(LLCHAR))
            NLLCHAR2BUFFER(_buffer, _index, _start, type_id)
            _start += type_id;
            break;
        case DataType<LSTRING, LLog::SupportedTypes >::value: {
            LSTRING data = *reinterpret_cast<LSTRING*>(_start); _start += sizeof(LSTRING);
            DECODESTRING(_buffer, _index, data._str)
            break;
        }
        case DataType<LLINT16, LLog::SupportedTypes >::value:
            CHECKMEM(_buffer, _index, _size, 18 * sizeof(LLCHAR))
            _index += Tool::num2string((LLINT64)*reinterpret_cast<LLINT16*>(_start), _buffer + _index); 
            _start += sizeof(LLINT16);
            break;
        case DataType<LLINT32, LLog::SupportedTypes >::value:
            CHECKMEM(_buffer, _index, _size, 18 * sizeof(LLCHAR))
            _index += Tool::num2string((LLINT64)*reinterpret_cast<LLINT32*>(_start), _buffer + _index); 
            _start += sizeof(LLINT32);
            break;
        case DataType<LLINT64, LLog::SupportedTypes >::value:
            CHECKMEM(_buffer, _index, _size, 18 * sizeof(LLCHAR))
            _index += Tool::num2string(*reinterpret_cast<LLINT64*>(_start), _buffer + _index); 
            _start += sizeof(LLINT64);
            break;
        case DataType<LUINT16, LLog::SupportedTypes >::value:
            CHECKMEM(_buffer, _index, _size, 18 * sizeof(LLCHAR))
            _index += Tool::num2string((LUINT64)*reinterpret_cast<LUINT16*>(_start), _buffer + _index); 
            _start += sizeof(LUINT16);
            break;
        case DataType<LUINT32, LLog::SupportedTypes >::value:
            CHECKMEM(_buffer, _index, _size, 18 * sizeof(LLCHAR))
            _index += Tool::num2string((LUINT64)*reinterpret_cast<LUINT32*>(_start), _buffer + _index); 
            _start += sizeof(LUINT32);
            break;
        case DataType<LUINT64, LLog::SupportedTypes >::value:
            CHECKMEM(_buffer, _index, _size, 18 * sizeof(LLCHAR))
            _index += Tool::num2string(*reinterpret_cast<LUINT64*>(_start), _buffer + _index); 
            _start += sizeof(LUINT64);
            break;
        default:
            break;
        }
    }
}

LLog::Stream::Stream(LUINT32 _size)
    : __buffer(_size) {
}

LLog::Stream::~Stream() {
}

LLCHAR*
LLog::Stream::buffer_begin() {
    return LLog::__buffer::buffer_begin();
}

LUINT32 
LLog::Stream::index_add(LUINT32 _size) {
    return LLog::__buffer::index_add(_size);
}

void 
LLog::Stream::operator<<(LLCHAR _val) {
    encode<LLCHAR>(_val, DataType < LLCHAR, LLog::SupportedTypes >::value);
}

void 
LLog::Stream::operator<<(LLCHAR* _val) {
    encodeString(_val);
}

void 
LLog::Stream::operator<<(const LLCHAR* _val) {
    encodeString(_val);
}

void 
LLog::Stream::operator<<(LLINT16 _val) {
    encode<LLINT16>(_val, DataType < LLINT16, LLog::SupportedTypes >::value);
}

void 
LLog::Stream::operator<<(LUINT16 _val) {
    encode<LUINT16>(_val, DataType < LUINT16, LLog::SupportedTypes >::value);
}

void 
LLog::Stream::operator<<(LLINT32 _val) {
    encode<LLINT32>(_val, DataType < LLINT32, LLog::SupportedTypes >::value);
}

void 
LLog::Stream::operator<<(LUINT32 _val) {
    encode<LUINT32>(_val, DataType < LUINT32, LLog::SupportedTypes >::value);
}

void 
LLog::Stream::operator<<(LLINT64 _val) {
    encode<LLINT64>(_val, DataType < LLINT64, LLog::SupportedTypes >::value);
}

void 
LLog::Stream::operator<<(LUINT64 _val) {
    encode<LUINT64>(_val, DataType < LUINT64, LLog::SupportedTypes >::value);
}

void 
LLog::Stream::operator<<(LLFLOAT _val) {
    encode<LLFLOAT>(_val, DataType < LLFLOAT, LLog::SupportedTypes >::value);
}

void 
LLog::Stream::operator<<(LLDOUBLE _val) {
    encode<LLDOUBLE>(_val, DataType < LLDOUBLE, LLog::SupportedTypes >::value);
}

void 
LLog::Stream::encodeString(const LLCHAR* _val) {
    LUINT32 const _len = strlen(_val);
    is_needed_resize(_len + sizeof(LUINT8) + sizeof(LUINT32));
    encode<LUINT8>(DataType < LLCHAR*, LLog::SupportedTypes >::value);
    encode<LUINT32>(_len);
    LLCHAR* _b = &buffer_begin()[index_add(_len)];
    memcpy(_b, _val, _len);
}

LLCHAR*
LLog::Stream::decode2Buffer(LUINT32 & __size) {
    LLCHAR* _buffer = buffer_begin();
    const LLCHAR* const end = _buffer + index_add();
    LUINT64 timestamp = *reinterpret_cast<LUINT64*>(_buffer); _buffer += sizeof(LUINT64); // len 26
    LSTRING host      = *reinterpret_cast<LSTRING*>(_buffer); _buffer += sizeof(LSTRING); // len 9
    LUINT32 pid       = *reinterpret_cast<LUINT32*>(_buffer); _buffer += sizeof(LUINT32); // len 7
    LUINT32 tid       = *reinterpret_cast<LUINT32*>(_buffer); _buffer += sizeof(LUINT32); // len 7
    LLINT32 level     = *reinterpret_cast<LLINT32*>(_buffer); _buffer += sizeof(LLINT32); // len 9
    LSTRING file      = *reinterpret_cast<LSTRING*>(_buffer); _buffer += sizeof(LSTRING); // len 9
    LSTRING func      = *reinterpret_cast<LSTRING*>(_buffer); _buffer += sizeof(LSTRING); // len 9
    LLINT32 line      = *reinterpret_cast<LLINT32*>(_buffer); _buffer += sizeof(LLINT32); // len 5

    LLCHAR* __buffer = (LLCHAR*)malloc((120 + host._size) * 2);
    __size  = 26;
    Tool::format_timestamp(__buffer, timestamp);
    __buffer[__size++] = ' ';
    NLLCHAR2BUFFER(__buffer, __size, host._str, host._size)
    __buffer[__size++] = ':';
    __size += Tool::num2string((LLINT64)pid, __buffer + __size);
    __buffer[__size++] = '_';
    __size += Tool::num2string((LLINT64)tid, __buffer + __size);
    __buffer[__size++] = ' ';
    __buffer[__size++] = '[';
    NLLCHAR2BUFFER(__buffer, __size, LLog::llog_tags[level], 9)
    __buffer[__size++] = ']';
    __buffer[__size++] = '(';
    DECODESTRING(__buffer, __size, file._str)
    __buffer[__size++] = ':';
    __buffer[__size++] = ':';
    DECODESTRING(__buffer, __size, func._str)
    __buffer[__size++] = ':';
    __size += Tool::num2string((LLINT64)line, __buffer + __size);
    __buffer[__size++] = ')';
    __buffer[__size++] = ' ';

    stringify(__buffer, __size, (120 + host._size) * 2, _buffer, end);

    return __buffer;
}