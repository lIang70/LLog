#include "buffer.h"

LLog::buffer_base::buffer_base(LUINT32 _size)
    : m_nBufferCap(m_nFixedSize), m_nBufferI(0) {
    is_needed_resize(_size < m_nFixedSize ? 0 : _size - m_nFixedSize);
}

LLog::buffer_base::buffer_base(const buffer_base& _another) noexcept
    : m_nBufferCap(m_nFixedSize), m_nBufferI(0) {
    if (!is_needed_resize(_another.m_nBufferI)) {
        memcpy(m_cStackBuffer, _another.m_cStackBuffer, _another.m_nBufferI);
    } else {
        memcpy(m_uHeapBuffer.get(), _another.m_uHeapBuffer.get(), _another.m_nBufferI);
    }
    m_nBufferI = _another.m_nBufferI;
}

LLog::buffer_base::buffer_base(buffer_base&& _another) noexcept
    : m_nBufferCap(m_nFixedSize), m_nBufferI(0) {
    if (!is_needed_resize(_another.m_nBufferI)) {
        memcpy(m_cStackBuffer, _another.m_cStackBuffer, _another.m_nBufferI);
    } else {
        m_uHeapBuffer.swap(_another.m_uHeapBuffer);
    }
    m_nBufferI = _another.m_nBufferI;
}

LLog::buffer_base&
LLog::buffer_base::operator=(const buffer_base& _another) noexcept {
    m_nBufferI = 0;
    if (!is_needed_resize(_another.m_nBufferI) && !m_uHeapBuffer && !_another.m_uHeapBuffer) {
        memcpy(m_cStackBuffer, _another.m_cStackBuffer, _another.m_nBufferI);
    } else if (_another.m_uHeapBuffer) {
        memcpy(m_uHeapBuffer.get(), _another.m_uHeapBuffer.get(), _another.m_nBufferI);
    } else {
        memcpy(m_uHeapBuffer.get(), _another.m_cStackBuffer, _another.m_nBufferI);
    }
    m_nBufferI = _another.m_nBufferI;
    return *this;
}

LLog::buffer_base&
LLog::buffer_base::operator=(buffer_base&& _another) noexcept {
    m_nBufferI = 0;
    if (!is_needed_resize(_another.m_nBufferI) && !m_uHeapBuffer && !_another.m_uHeapBuffer) {
        memcpy(m_cStackBuffer, _another.m_cStackBuffer, _another.m_nBufferI);
    } else if (_another.m_uHeapBuffer) {
        m_uHeapBuffer.swap(_another.m_uHeapBuffer);
    } else {
        memcpy(m_uHeapBuffer.get(), _another.m_cStackBuffer, _another.m_nBufferI);
    }
    m_nBufferI = _another.m_nBufferI;

    return *this;
}

void
LLog::buffer_base::reset() {
    m_nBufferI = 0;
}

LUINT32
LLog::buffer_base::index_add(LUINT32 _size) {
    m_nBufferI += _size;
    return m_nBufferI - _size;
}

LLCHAR*
LLog::buffer_base::buffer_begin() {
    return !m_uHeapBuffer ? m_cStackBuffer : m_uHeapBuffer.get();
}

LUINT32
LLog::buffer_base::cap() const {
    return m_nBufferCap;
}

LLBOOL
LLog::buffer_base::is_needed_resize(LUINT32 _size) {
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
LLog::buffer_base::swap(buffer_base& _buffer) {
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

LLog::Stream::Stream(LUINT32 _size)
    : buffer_base(_size) {
}

LLog::Stream::~Stream() = default;

LLCHAR*
LLog::Stream::buffer_begin() {
    return LLog::buffer_base::buffer_begin();
}

LUINT32 
LLog::Stream::index_add(LUINT32 _size) {
    return LLog::buffer_base::index_add(_size);
}

LLog::Stream &  
LLog::Stream::operator<<(LLCHAR _val) {
    is_needed_resize(sizeof(LLCHAR));
    LLINT32 _index = index_add(1);
    *(buffer_begin() + _index) = _val;
    return *this;
}

LLog::Stream &  
LLog::Stream::operator<<(LLCHAR* _val) {
    register LUINT32 _len = strlen(_val);
    is_needed_resize(_len * sizeof(LLCHAR));
    register LLINT32 _index = index_add(_len);
    memcpy(buffer_begin() + _index, _val, _len); 
    return *this;
}

LLog::Stream &  
LLog::Stream::operator<<(const LLCHAR* _val) {
    register LUINT32 _len = strlen(_val);
    is_needed_resize(_len * sizeof(LLCHAR));
    register LLINT32 _index = index_add(_len);
    memcpy(buffer_begin() + _index, _val, _len); 
    return *this;
}

LLog::Stream &   
LLog::Stream::operator<<(LLINT16 _val) {
    is_needed_resize((LLINT64_LEN + 3) * sizeof(LLCHAR));
    register LLINT32 _index = index_add(0);
    register LLINT32 _size  = Tool::num2string((LLINT64)_val, buffer_begin() + _index);
    index_add(_size);
    return *this;
}

LLog::Stream &   
LLog::Stream::operator<<(LUINT16 _val) {
    is_needed_resize((LLINT64_LEN + 3) * sizeof(LLCHAR));
    register LLINT32 _index = index_add(0);
    register LLINT32 _size  = Tool::num2string((LUINT64)_val, buffer_begin() + _index);
    index_add(_size);
    return *this;
}

LLog::Stream &  
LLog::Stream::operator<<(LLINT32 _val) {
    is_needed_resize((LLINT64_LEN + 3) * sizeof(LLCHAR));
    register LLINT32 _index = index_add(0);
    register LLINT32 _size  = Tool::num2string((LLINT64)_val, buffer_begin() + _index);
    index_add(_size);
    return *this;
}

LLog::Stream &   
LLog::Stream::operator<<(LUINT32 _val) {
    is_needed_resize((LLINT64_LEN + 3) * sizeof(LLCHAR));
    register LLINT32 _index = index_add(0);
    register LLINT32 _size  = Tool::num2string((LUINT64)_val, buffer_begin() + _index);
    index_add(_size);
    return *this;
}

LLog::Stream &   
LLog::Stream::operator<<(LLINT64 _val) {
    is_needed_resize((LLINT64_LEN + 3) * sizeof(LLCHAR));
    register LLINT32 _index = index_add(0);
    register LLINT32 _size  = Tool::num2string((LLINT64)_val, buffer_begin() + _index);
    index_add(_size);
    return *this;
}

LLog::Stream &   
LLog::Stream::operator<<(LUINT64 _val) {
    is_needed_resize((LLINT64_LEN + 3) * sizeof(LLCHAR));
    register LLINT32 _index = index_add(0);
    register LLINT32 _size  = Tool::num2string((LUINT64)_val, buffer_begin() + _index);
    index_add(_size);
    return *this;
}

LLog::Stream &   
LLog::Stream::operator<<(LLFLOAT _val) {
    // todo
    return *this;
}

LLog::Stream &   
LLog::Stream::operator<<(LLDOUBLE _val) {
    // todo
    return *this;
}