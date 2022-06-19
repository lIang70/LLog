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

    inline void   setDelete(LLBOOL _delete = true) { m_bCanDelete = _delete; }
    inline LLBOOL canBeDeleted()                   { return m_bCanDelete; }

    LUINT32 try_pop(LLCHAR * _buffer, LUINT32 _size) {
        LUINT64 __index = m_aTail.load();
        if (__index <= m_aHead) return false;
        LUINT32 _sum = 0;
        while (m_aHead <= __index) {
            if (_sum + LOGLENTHMAX >= _size) return _sum;
            BufferE & _e = m_pBuffer[m_aHead % m_nBufferCap];
            SpinLock _lock(_e._flag);
            if (_e._status == EMPTY_STATUS) return _sum;
            memcpy(_buffer + _sum, _e._buffer.buffer_begin(), _e._buffer.index_add());
            _sum += _e._buffer.index_add();
            _e._status = EMPTY_STATUS;
            m_aHead++;

#ifdef _DEBUG
            pop_count_add();
#endif
        }
        return _sum;
    }
};
LLOG_SPACE_END()

#endif // !LLOG_STREAMRING_H