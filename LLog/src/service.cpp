#include "service.h"
#include "buffer.h"
#include "file.h"
#include "queue.h"

LLog::Service* LLog::Service::m_pIns = nullptr;

LLog::Service::Service() 
    : m_nStatus(SERIVCE_IDLE),
      m_nThreadNum(2),
      m_pStreamQueue(new StreamBuffer),
      m_pFile(new File) {
#ifdef C_OS_WIN
    m_nPID = GetCurrentProcessId();
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerName(m_cHostName, &size);
#elif defined(C_OS_LINUX)
    m_nPID = getpid();
    gethostname(m_cHostName, sizeof(m_cHostName));
#endif // C_OS_WIN
    m_sHost._str = m_cHostName;
    m_sHost._size = strlen(m_cHostName);

    LLCHAR  _curFile[FILEPATHLEN];
    LUINT64 _time = getTime();
    std::time_t time_t = _time / 1000000;
    tm gmtime = *std::gmtime(&time_t);
    sprintf(_curFile, "%04d_%02d_%02d_%02d_%02d_%02d_%s_common", 
            gmtime.tm_year + 1900, gmtime.tm_mon + 1, gmtime.tm_mday,
            gmtime.tm_hour, gmtime.tm_min, gmtime.tm_sec, m_cHostName);

    m_pFile->setFileName(_curFile);
}

LLog::Service::~Service() {
    terminal();
}

LLog::Service* 
LLog::Service::getIns() {
    static std::once_flag oc;
    std::call_once(oc, [&] {  m_pIns = new LLog::Service; });
    return m_pIns;
}

LUINT32 
LLog::Service::getPID() const {
    return m_nPID;
}

LUINT32 
LLog::Service::getTID() {
#ifdef C_OS_WIN
    static thread_local const LUINT32 id = GetCurrentThreadId();
#elif defined(C_OS_LINUX)
    static thread_local const LUINT32 id = gettid();
#endif // C_OS_WIN
    return id;
}

LUINT64 
LLog::Service::getTime() {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();;
}

const LLCHAR* 
LLog::Service::getHostName() const {
    return m_cHostName;
}

const LLog::LSTRING &
LLog::Service::getHost() const {
    return m_sHost;
}

void
LLog::Service::setThreadNum(LUINT32 _threadNum) {
    m_nThreadNum = LMIN(_threadNum, THREADMAX);
}

LUINT32 
LLog::Service::exec() {
    try {
        m_nStatus = SERIVCE_RUNNING;
        auto _func = [&] {
            LLog::Stream _stream;
            LUINT32      _size;
            while (true) {
                if (m_pStreamQueue->try_pop(_stream)) {
                    auto _buffer = _stream.decode2Buffer(_size);
                    m_mFileLock.lock();
                    m_pFile->writeBuffer(_buffer, _size);
                    m_mFileLock.unlock();
                    delete[] _buffer;
                } else if (m_nStatus == SERIVCE_STOP) {
                    break;
                } else {
                    std::this_thread::sleep_for(std::chrono::microseconds(50));
                }
            }
        };

        for (LUINT32 _size = 0; _size < m_nThreadNum; ++_size) {
            m_tStreamHandle[_size] = std::make_shared<std::thread>(_func);
        }
    } catch(const std::exception& e) {
        m_nStatus = SERIVCE_ERROR;
        printf("[LLog Fatal] %s\n", e.what());
        return (SERIVCE_ERROR);
    }
    return (ZERO);
}

LUINT32
LLog::Service::terminal() {
    try {
        m_nStatus = SERIVCE_STOP;
        for (LUINT32 _size = 0; _size < m_nThreadNum; ++_size) {
            if (m_tStreamHandle[_size]->joinable()) {
                m_tStreamHandle[_size]->join();
            }
        }
        delete m_pStreamQueue;
        delete m_pFile;
    } catch(const std::exception& e) {
        m_nStatus = SERIVCE_ERROR;
        printf("[LLog Fatal] %s\n", e.what());
        return (SERIVCE_ERROR);
    }
    return (ZERO);
}

void
LLog::Service::push(LLog::Stream* _stream) {
    m_pStreamQueue->push(std::move(*_stream));
}
