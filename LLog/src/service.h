#ifndef LLOG_SERVICE_H
#define LLOG_SERVICE_H

#include "global.h"
#include "queue.h"

#define SERIVCE_ERROR   (-1)
#define SERIVCE_IDLE    (0)
#define SERIVCE_STOP    (1)
#define SERIVCE_RUNNING (2)

class File;

LLOG_SPACE_BEGIN()

    class Stream;
    typedef RingBuffer<Stream> StreamBuffer;
        
    class Service {
        static Service* m_pIns;

        LLINT8      m_nStatus;
        LLINT32     m_nPID;
        LLCHAR      m_cHostName[HOSTNAMELEN];
        LSTRING     m_sHost;

        StreamBuffer    *m_pStreamQueue;
        LUINT32         m_nThreadNum;
        shared_thread   m_tStreamHandle[THREADMAX];

        File        *m_pFile;
        std::mutex  m_mFileLock;

    private:
        Service();
        ~Service();

    public:
        static Service* getIns();

        LUINT32         getPID() const;
        LUINT32         getTID();
        LUINT64         getTime();
        const LLCHAR*   getHostName() const;
        const LSTRING & getHost() const;

        void    setThreadNum(LUINT32 _threadNum);
        LUINT32 exec();
        LUINT32 terminal();
        void    push(Stream * _stream);

    };

LLOG_SPACE_END()

#endif // !LLOG_SERVICE_H
