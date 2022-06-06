#ifndef LLOG_SERVICE_H
#define LLOG_SERVICE_H

#include "global.h"

#define SERIVCE_ERROR   (-1)
#define SERIVCE_IDLE    (0)
#define SERIVCE_STOP    (1)
#define SERIVCE_RUNNING (2)

class File;

LLOG_SPACE_BEGIN()

    class buffer_base;
    class Stream;
    class StreamRing;
        
    class Service {

        class ring_flag {
        public:
            explicit ring_flag() {};
            ~ring_flag();
        };

        static __thread     StreamRing* s_sStreamQueue;
        static thread_local ring_flag   s_rQueueFlag;
        static              Service*    s_pIns;

        LLINT8      m_nStatus;
        LLINT32     m_nPID;
        LLCHAR      m_cHostName[HOSTNAMELEN];
        LSTRING     m_sHost;

        std::mutex                  m_mBufferLock;
        std::vector<StreamRing*>    m_vLocalBuffer{};
        LLINT32                     m_nBufferCount = 0;

        buffer_base *   m_bLogBuffer;

        shared_thread   m_tStreamHandle;
        
        File *  m_pFile;

    private:
        Service();
        ~Service();

        void allocatedLocalBuffer();
        void mainThread();

    public:
        static Service* getIns();
        static LUINT32  exec();
        static LUINT32  terminal();

        LUINT32         getPID() const;
        LUINT32         getTID();
        LUINT64         getTime();
        const LLCHAR*   getHostName() const;
        const LSTRING & getHost() const;

        void    push(Stream * _stream);

    };

LLOG_SPACE_END()

#endif // !LLOG_SERVICE_H
