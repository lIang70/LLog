#ifndef LLOG_H
#define LLOG_H

#include "llog_global.h"

LLOG_SPACE_BEGIN()
    class Stream;

    class Line {
    private:
        LLINT32 m_nLevel;
        Stream* m_pStream;

    public:
        /**
         * @brief Construct a new Line object of LLog to output log.
         * 
         * @param level log level.
         * @param file  current file.
         * @param func  current func.
         * @param line  current line.
         */
        Line(LLINT32 level = LLog::DEBUG,
            const LLCHAR* file = nullptr, const LLCHAR* func = nullptr, const LLINT32 line = 0);
        ~Line();

        /**
         * @brief Enter log data into cache of LLog::Line.
         */
        Line& append(LLCHAR* format, ...);
        Line& operator<<(LLCHAR val);
        Line& operator<<(LLCHAR* val);
        Line& operator<<(const LLCHAR* val);
        Line& operator<<(LLINT16 val);
        Line& operator<<(LUINT16 val);
        Line& operator<<(LLINT32 val);
        Line& operator<<(LUINT32 val);
        Line& operator<<(LLINT64 val);
        Line& operator<<(LUINT64 val);
        Line& operator<<(LLFLOAT val);
        Line& operator<<(LLDOUBLE val);
        
    };

    /**
     * @brief Get the current level output of LLog.
     * 
     * @return Level of LLog output.
     */
    Level logLevel();

    /**
     * @brief Set the Log Level object.
     * 
     * @param level the level you set to LLog.
     */
    void setLogLevel(Level level = Level::DEBUG);

    /**
     * @brief Set the Thread Num object.
     * 
     * @param _threadNum 
     */
    void setThreadNum(LUINT32 _threadNum = 2);

    /**
     * @brief LLog service start.
     * 
     * @return LLINT32 
     */
    LLINT32 start();

    /**
     * @brief LLog service terminal.
     * 
     * @return LLINT32 
     */
    LLINT32 terminal();

LLOG_SPACE_END()

#ifndef __REL_FILE__
#include <cstring>
#ifdef C_OS_WIN
#define __REL_FILE__ { strrchr(__FILE__,'\\') ? strrchr(__FILE__,'\\') + 1 : __FILE__ }
#elif defined(C_OS_LINUX)
#define __REL_FILE__ { strrchr(__FILE__,'/') ? strrchr(__FILE__,'/') + 1 : __FILE__ }
#endif // C_OS_WIN
#endif
#define LLOG(level)                                             \
    if (LLog::logLevel() <= level)                              \
        LLog::Line(level, __REL_FILE__, __FUNCTION__, __LINE__)
#define lLog(level, format, ...)                                \
    if (LLog::logLevel() <= level)                              \
        LLog::Line(level, __REL_FILE__, __FUNCTION__, __LINE__).append(format, __VA_ARGS__)

#define llDebug     LLOG(LLog::Level::DEBUG)
#define llInfo      LLOG(LLog::Level::INFO)
#define llWarning   LLOG(LLog::Level::WARNING)
#define llError     LLOG(LLog::Level::ERROR)
#define llCritical  LLOG(LLog::Level::CRITICAL)
#define llFatal     LLOG(LLog::Level::FATAL)

#endif // !LLOG_H
