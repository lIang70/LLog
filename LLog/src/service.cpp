#include "service.h"
#include "buffer.h"
#include "file.h"
#include "streamring.h"

#define __BUFFER_SIZE__ (1024 * 1024 * 16)

__thread        LLog::StreamRing*           LLog::Service::s_sStreamQueue = nullptr;
                LLog::Service*              LLog::Service::s_pIns = nullptr;
thread_local    LLog::Service::ring_flag    LLog::Service::s_rQueueFlag;

LLog::Service::ring_flag::~ring_flag() {
    if (s_sStreamQueue != nullptr) {
        s_sStreamQueue->setDelete();
        s_sStreamQueue = nullptr;
    }
}

LLog::Service::Service() 
    : m_nStatus(SERIVCE_IDLE),
      m_nBufferCount(0),
      m_bLogBuffer(new buffer_base(__BUFFER_SIZE__)),
      m_pFile(new File) {
    /**
     * > Init hostname and process id.
     */
    Tool::init_system_info();

    /**
     * > Init name of the log file.
     */
    LLCHAR  _curFile[FILEPATHLEN];
    LUINT64 _time = Tool::get_system_time();
    std::time_t time_t = _time / 1000000;
    tm* gmtime = std::gmtime(&time_t);
    sprintf(_curFile, "%04d_%02d_%02d_%02d_%02d_%02d_%s_common", 
            gmtime->tm_year + 1900, gmtime->tm_mon + 1, gmtime->tm_mday,
            gmtime->tm_hour, gmtime->tm_min, gmtime->tm_sec, Tool::get_hostname());
    m_pFile->setFileName(_curFile);
}

LLog::Service::~Service() {
    m_nStatus = SERIVCE_STOP;
    if (m_tStreamHandle->joinable()) {
        m_tStreamHandle->join();
    }

    delete m_bLogBuffer;
    delete m_pFile;
}

void 
LLog::Service::allocatedLocalBuffer() {
    if (s_sStreamQueue == nullptr) {
        LLog::unique_mutex __guard(m_mBufferLock);
        m_nBufferCount++;
        __guard.unlock();
        s_sStreamQueue = new StreamRing();
        /**
         * > Weird C++ hack;  C++ thread_local are instantiated upon 
         * first use thus the s_rQueueFlag has to do this in order 
         * to instantiate this object.
         */
        s_rQueueFlag; 
        __guard.lock();
        m_vLocalBuffer.push_back(s_sStreamQueue);
    }
}

void 
LLog::Service::mainThread() {
    /**
     * > Index of the last StreamRing checked 
     * for uncompressed log messages
     */
    LLINT32 _lastBufferChecked = 0;

    /**
     * > Indicates whether a compression operation 
     * failed or not due to insufficient space in the outputBuffer
     */
    LLBOOL _outputBufferFull = false;

    /**
     * > Indicates that in scanning the local_thread StreamRings, 
     * mark true if all buffers have been cleaned
     */
    bool _bufferclear = false;

    while (m_nStatus != SERIVCE_STOP || 
           m_bLogBuffer->index_add() != 0 ||
           !_bufferclear) {
        
        _bufferclear = false;

        {
            LLog::unique_mutex __guard(m_mBufferLock);
            LLINT32 _index = _lastBufferChecked;

            /**
             * > Scan through the threadBuffers looking for log messages
             * to compress while the output buffer is not full.
             */
            while (!_outputBufferFull && !m_vLocalBuffer.empty()) {
                LLog::StreamRing* _ringBuffer = m_vLocalBuffer[_index];

                /*> If there's work, unlock to perform it */
                if (!_ringBuffer->canBeDeleted() || !_ringBuffer->empty()) {
                    __guard.unlock();
                    
                    LLCHAR* _begin = m_bLogBuffer->buffer_begin() + m_bLogBuffer->index_add();
                    LUINT32 _size  = m_bLogBuffer->cap() - m_bLogBuffer->index_add();
                    LLINT32 _count = _ringBuffer->try_pop(_begin, _size);
                    if (_count > 0) {
                        m_bLogBuffer->index_add(_count);
                    }
                    
                    if (m_bLogBuffer->index_add() + LOGLENTHMAX >= m_bLogBuffer->cap()) {
                        _lastBufferChecked = _index;
                        _outputBufferFull = true;
                    }
                    
                    __guard.lock();
                } else {
                    delete _ringBuffer;

                    m_vLocalBuffer.erase(m_vLocalBuffer.begin() + _index);
                    if (m_vLocalBuffer.empty()) {
                        _lastBufferChecked = _index = 0;
                        _bufferclear = true;
                        break;
                    }

                    /**
                     * > Back up the indexes so that we ensure we wont 
                     * skip a buffer in our pass (and it's okay to redo one)
                     */
                    if (_lastBufferChecked >= _index &&
                        _lastBufferChecked > 0) {
                        --_lastBufferChecked;
                    }
                    --_index;
                }

                _index = (_index + 1) % m_vLocalBuffer.size();

                /*> Completed a full pass through the buffers. */
                if (_index == _lastBufferChecked)
                    break;
            }
        }

        if (_outputBufferFull) {
            m_pFile->writeBuffer(m_bLogBuffer->buffer_begin(), m_bLogBuffer->index_add());
            m_bLogBuffer->reset();
            _outputBufferFull = false;
        } else if (_bufferclear) {
            m_pFile->writeBuffer(m_bLogBuffer->buffer_begin(), m_bLogBuffer->index_add());
            m_bLogBuffer->reset();
            _outputBufferFull = false;
        }
    }
}

LLog::Service* 
LLog::Service::getIns() {
    int i = sizeof(StreamRing);
    static std::once_flag oc;
    std::call_once(oc, [&] {  s_pIns = new LLog::Service; });
    return s_pIns;
}

LUINT32 
LLog::Service::exec() {
    try {
        if (s_pIns == nullptr) 
            LLog::Service::getIns();
        s_pIns->m_nStatus       = SERIVCE_RUNNING;
        s_pIns->m_tStreamHandle = std::make_shared<std::thread>(&LLog::Service::mainThread, s_pIns);

    } catch(const std::exception& e) {
        s_pIns->m_nStatus = SERIVCE_ERROR;
        printf("[LLog Fatal] %s\n", e.what());
        return (SERIVCE_ERROR);
    }
    return (ZERO);
}

LUINT32
LLog::Service::terminal() {
    try {
        delete s_pIns;
    } catch(const std::exception& e) {
        printf("[LLog Fatal] %s\n", e.what());
        return (SERIVCE_ERROR);
    }
    return (ZERO);
}

void
LLog::Service::push(LLog::Stream* _stream) {
    if (s_sStreamQueue == nullptr) allocatedLocalBuffer();
    s_sStreamQueue->push(std::move(*_stream));
}
