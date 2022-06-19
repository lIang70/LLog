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

        LLINT8  m_nStatus;

        std::mutex                  m_mBufferLock;
        std::vector<StreamRing*>    m_vLocalBuffer{};
        LLINT32                     m_nBufferCount = 0;
        shared_thread               m_tStreamHandle;

        buffer_base *   m_bLogBuffer;
        
        File *  m_pFile;

    private:
        Service();
        ~Service();

        void allocatedLocalBuffer();
        void mainThread();

    public:
        static  Service*    getIns();
        static  LUINT32     exec();
        static  LUINT32     terminal();

                void        push(Stream * _stream);

    };

LLOG_SPACE_END()

#endif // !LLOG_SERVICE_H
