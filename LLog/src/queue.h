#ifndef LLOG_QUEUE_H
#define LLOG_QUEUE_H

#include "global.h"

#define _DEBUG

#define RING_BUFFER_LENGTH  (1<<16)
#define EMPTY_STATUS        (0)
#define WROTE_STATUS        (1)

class SpinLock {
    std::atomic_flag & m_aFlag;

public:
    SpinLock(std::atomic_flag & flag) 
        : m_aFlag(flag) {
        while (m_aFlag.test_and_set(std::memory_order_acquire));
    }
    ~SpinLock() { m_aFlag.clear(std::memory_order_release); }
};

template<typename _Ty>
class RingBuffer {

    typedef struct BufferE {
        std::atomic_flag    _flag;
        _Ty                 _buffer;
        LUINT8              _status = EMPTY_STATUS;
    } BufferE;

    BufferE*                m_pBuffer = nullptr;
    LUINT32                 m_nBufferCap = 0;
    LUINT32                 m_nHalfCap = 0;
    std::atomic_uint64_t    m_aHead;
    std::atomic_uint64_t    m_aTail;
    std::mutex              m_mTailLock;

#ifdef _DEBUG
    std::atomic_uint64_t    __push_count;
    std::atomic_uint64_t    __pop_count;
#endif

public:
    RingBuffer(LUINT32 _size = RING_BUFFER_LENGTH) {
        m_nBufferCap = _size;
        m_nHalfCap = m_nBufferCap>>1;
        m_pBuffer = new BufferE[_size];
        m_aHead.exchange(0, std::memory_order_relaxed);
        m_aTail.exchange(0, std::memory_order_relaxed);
#ifdef _DEBUG
        __push_count.exchange(0, std::memory_order_relaxed);
        __pop_count.exchange(0, std::memory_order_relaxed);
#endif
    }

    ~RingBuffer() {
        delete[] m_pBuffer;

#ifdef _DEBUG
        printf("[RingBuffer] push items : %ld\n", __push_count.load());
        printf("[RingBuffer] pop  items : %ld\n", __pop_count.load());
#endif
    }

    void push(_Ty && _data) {
        LLog::mutex_guard __guard(m_mTailLock);
        while(m_aTail.load() - m_aHead.load() > m_nHalfCap);
        register LUINT64 _index = m_aTail.fetch_add(1, std::memory_order_relaxed) % m_nBufferCap;
        BufferE & _e = m_pBuffer[_index];
        SpinLock _lock(_e._flag);
        _e._buffer = std::move(_data);
        _e._status = WROTE_STATUS;
#ifdef _DEBUG
        __push_count++;
#endif
    }

    void push_unlock(_Ty && _data) {
        while(m_aTail.load() - m_aHead.load() > m_nHalfCap);
        register LUINT64 _index = m_aTail.fetch_add(1, std::memory_order_relaxed) % m_nBufferCap;
        BufferE & _e = m_pBuffer[_index];
        SpinLock _lock(_e._flag);
        _e._buffer = std::move(_data);
        _e._status = WROTE_STATUS;
#ifdef _DEBUG
        __push_count++;
#endif
    }

    LLBOOL try_pop(_Ty & _data) {
        BufferE & _e = m_pBuffer[m_aHead % m_nBufferCap];
        SpinLock _lock(_e._flag);
        if (_e._status == EMPTY_STATUS) return false;
        _data = std::move(_e._buffer);
        _e._status = EMPTY_STATUS;
        m_aHead++;
#ifdef _DEBUG
        __pop_count++;
#endif

        return true;
    }

};

#endif // !LLOG_QUEUE_H