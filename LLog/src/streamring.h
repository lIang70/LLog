#ifndef LLOG_STREAMRING_H
#define LLOG_STREAMRING_H

#include "buffer.h"
#include "ringbuffer.h"

LLOG_SPACE_BEGIN()
class StreamRing : public RingBuffer<Stream> {
    LLBOOL m_bCanDelete = false;
public:
    StreamRing(LUINT32 _size = RING_BUFFER_LENGTH) 
        : RingBuffer<Stream>(_size) {
    }
    ~StreamRing() = default;

    virtual inline void   setDelete(LLBOOL _delete = true) { m_bCanDelete = _delete; }
    virtual inline LLBOOL canBeDeleted()                   { return m_bCanDelete; }

    inline LUINT32 try_pop(LLCHAR * _buffer, LUINT32 _size) {
        LUINT64 __index = m_aTail.load();
        if (__index <= m_aHead) return false;
        LUINT32 _sum = 0;
        while (m_aHead <= __index) {
            if (_sum + LOGLENTHMAX >= _size) return _sum;
            BufferE & _e = m_pBuffer[m_aHead % m_nBufferCap];
            SpinLock _lock(_e._flag);
            if (_e._status == EMPTY_STATUS) return _sum;
            LUINT32 _count = 0;
            _e._buffer.decode2Buffer(_buffer + _sum, _count);
            _e._status = EMPTY_STATUS;
            m_aHead++;
            _sum += _count;

#ifdef _DEBUG
            pop_count_add();
#endif
        }
        return _sum;
    }

    inline LUINT32 try_pop_compressed(LLCHAR * _buffer, LUINT32 _size) {
        LUINT64 __index = m_aTail.load();
        if (__index <= m_aHead) return false;
        LUINT32 _sum = 0;
        while (m_aHead <= __index) {
            if (_sum + LOGLENTHMAX >= _size) return _sum;
            BufferE & _e = m_pBuffer[m_aHead % m_nBufferCap];
            SpinLock _lock(_e._flag);
            if (_e._status == EMPTY_STATUS) return _sum;
            LUINT32 _count = _e._buffer.index_add();
            memcpy(_buffer + _sum, _e._buffer.buffer_begin(), _count);
            _e._status = EMPTY_STATUS;
            m_aHead++;
            _sum += _count;

#ifdef _DEBUG
            pop_count_add();
#endif
        }
        return _sum;
    }
};
LLOG_SPACE_END()

#endif // !LLOG_STREAMRING_H