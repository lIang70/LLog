#include "llog.h"
#include "service.h"
#include "buffer.h"

LLog::Line::Line(LLINT32 level, const LLCHAR* file, const LLCHAR* func, const LLINT32 line)
    : m_nLevel(level), m_pStream(new LLog::Stream()) {
    /*> log format: &time &hostname:&pid_&tid &level [&file:&func &line] &ctx */
    LLCHAR* _buffer = m_pStream->buffer_begin();
    m_pStream->index_add(26);
    Tool::format_timestamp(_buffer, Tool::get_system_time());
    *m_pStream << ' ' << Tool::get_hostname() << ':' << Tool::get_pid() << '_' << Tool::get_tid() << ' ';
    *m_pStream << level << " [" << file << ':' << func << ' ' << line << "] ";
}

LLog::Line::~Line() {
    *m_pStream << '\n';
    LLog::Service::getIns()->push(m_pStream);
    delete m_pStream;
}

LLog::Line& 
LLog::Line::append(LLCHAR* __format, ...) {
    // to do
    return *this;
}

LLog::Line& 
LLog::Line::operator<<(LLCHAR val) {
    *m_pStream << val;
    return *this;
}

LLog::Line& 
LLog::Line::operator<<(LLCHAR* val) {
    *m_pStream << val;
    return *this;
}

LLog::Line&
LLog::Line::operator<<(const LLCHAR* val) {
    *m_pStream << val;
    return *this;
}

LLog::Line& 
LLog::Line::operator<<(LLINT16 val) {
    *m_pStream << val;
    return *this;
}

LLog::Line& 
LLog::Line::operator<<(LUINT16 val) {
    *m_pStream << val;
    return *this;
}

LLog::Line& 
LLog::Line::operator<<(LLINT32 val) {
    *m_pStream << val;
    return *this;
}

LLog::Line& 
LLog::Line::operator<<(LUINT32 val) {
    *m_pStream << val;
    return *this;
}

LLog::Line& 
LLog::Line::operator<<(LLINT64 val) {
    *m_pStream << val;
    return *this;
}

LLog::Line& 
LLog::Line::operator<<(LUINT64 val) {
    *m_pStream << val;
    return *this;
}

LLog::Line& 
LLog::Line::operator<<(LLFLOAT val) {
    *m_pStream << val;
    return *this;
}

LLog::Line& 
LLog::Line::operator<<(LLDOUBLE val) {
    *m_pStream << val;
    return *this;
}


LLog::Level
LLog::logLevel() {

    return LLog::Level::DEBUG;
}

void 
LLog::setLogLevel(LLog::Level level) {
    
}

LLINT32 
LLog::start() {
    return LLog::Service::exec();
}

LLINT32
LLog::terminal() {
    return LLog::Service::terminal();
}

