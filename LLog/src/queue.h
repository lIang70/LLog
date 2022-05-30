#ifndef LLOG_QUEUE_H
#define LLOG_QUEUE_H

#include "global.h"

#define BUFFERQUEUE_LEN         2048
#define BUFFERQUEUE_NORMAL      0
#define BUFFERQUEUE_DESTROY     1

LLOG_SPACE_BEGIN()
    class __buffer;

    class BufferQueue {

        typedef struct BufferE {
            __buffer*           _buffer;
            std::atomic_bool    _flag;
            BufferE*            _next;
        } BufferE;

        BufferE*    m_pMsgQueue = nullptr;
        BufferE*    m_nReadIndex = nullptr;
        BufferE*    m_nWriteIndex = nullptr;
        LLINT32     m_nQueueCap = 0;
        LLBOOL      m_bFixedSize = false;
        std::condition_variable m_cv;
        std::mutex              m_mLock;
        LLINT32                 m_nStatus = BUFFERQUEUE_NORMAL;

    public:
        BufferQueue(LUINT32 _size = BUFFERQUEUE_LEN);
        ~BufferQueue();
        inline LLINT32  getStatus() { return m_nStatus; }
        inline void     setStatus(LLINT32 _status)  { m_nStatus = _status; }
        void        destory();
        void        notify();
        __buffer*   pop();
        void        push(__buffer* _buffer);
        void        setFixedSize(LLBOOL _is_fixed = true);
        LLBOOL      isFixedSize();

    };
LLOG_SPACE_END()

#endif // !LLOG_QUEUE_H