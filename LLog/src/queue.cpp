#include "queue.h"
#include "buffer.h"

#define SET_BUFFER(ptr, buffer) \
    ptr->_buffer = buffer;      \
    ptr->_flag.exchange(true);
#define SET_NULL(ptr)           \
    ptr->_buffer = nullptr;     \
    ptr->_flag.exchange(false);
#define NEXT_BUFFER(ptr)        \
    ptr = ptr->_next;
#define INSERT_BACK(ptr) {      \
    BufferE* _tmp = new BufferE;\
    _tmp->_flag.exchange(false);\
    _tmp->_buffer = nullptr;    \
    _tmp->_next = ptr->_next;   \
    ptr->_next = _tmp;          \
}

LLog::BufferQueue::BufferQueue(LUINT32 _size) {
    m_pMsgQueue = new BufferE;
    m_pMsgQueue->_flag.exchange(false);
    m_pMsgQueue->_next      = m_pMsgQueue;
    m_pMsgQueue->_buffer    = nullptr;
    m_nReadIndex            = m_pMsgQueue;
    m_nWriteIndex           = m_pMsgQueue;
    m_nQueueCap             = _size;

    for (LUINT32 i = 1; i < _size; i++)
        INSERT_BACK(m_nWriteIndex)
}

LLog::BufferQueue::~BufferQueue() {
    destory();
}

void 
LLog::BufferQueue::destory() {
    if (m_pMsgQueue == nullptr) return;

    while (m_pMsgQueue != m_pMsgQueue->_next) {
        BufferE* _tmp = m_pMsgQueue->_next;
        m_pMsgQueue->_next = _tmp->_next;
        delete[] _tmp;
    }
    delete[] m_pMsgQueue;
    m_pMsgQueue = nullptr;
}

void
LLog::BufferQueue::notify() {
    m_cv.notify_one();
}

LLog::__buffer*
LLog::BufferQueue::pop() {
    std::unique_lock<std::mutex> ul(m_mLock);  // condition_variable should be used with unique_lock
    m_cv.wait(ul, [&] { return (m_nWriteIndex != m_nReadIndex) || (m_nStatus == BUFFERQUEUE_DESTROY); });
    if (m_nWriteIndex != m_nReadIndex) {
        LLog::__buffer* b = m_nReadIndex->_buffer;
        SET_NULL(m_nReadIndex)
        NEXT_BUFFER(m_nReadIndex)
        return b;
    }
    return nullptr;
}

void 
LLog::BufferQueue::push(LLog::__buffer* _buffer) {
    std::lock_guard<std::mutex> _guard(m_mLock);
    if (m_nWriteIndex->_next == m_nReadIndex) { // notice: 有可能会出现链表过长
        m_nQueueCap++;
        INSERT_BACK(m_nWriteIndex)
    }
    SET_BUFFER(m_nWriteIndex, _buffer);
    NEXT_BUFFER(m_nWriteIndex);
    m_cv.notify_one();
}