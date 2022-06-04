#include "llog.h"
#include "service.h"
#include "buffer.h"

LLog::Line::Line(LLINT32 level,
    const LLCHAR* file, const LLCHAR* func, const LLINT32 line)
    : m_nLevel(level), m_pStream(new LLog::Stream()) {
    
    LLog::Service* service = LLog::Service::getIns();
    
    // log format: &time &hostname:&pid_&tid &level [&file:&func &line] &ctx
    m_pStream->encode<LUINT64>(service->getTime());
    m_pStream->encode<LSTRING>(service->getHost());
    m_pStream->encode<LUINT32>(service->getPID());
    m_pStream->encode<LUINT32>(service->getTID());
    m_pStream->encode<LLINT32>(level);
    m_pStream->encode<LSTRING>(LSTRING((LLCHAR*)file));
    m_pStream->encode<LSTRING>(LSTRING((LLCHAR*)func));
    m_pStream->encode<LLINT32>(line);
}

LLog::Line::~Line() {
    m_pStream->encode<LLCHAR>('\n', DataType < LLCHAR, LLog::SupportedTypes >::value);
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

void 
LLog::setThreadNum(LUINT32 _threadNum) {
    LLog::Service::getIns()->setThreadNum(_threadNum);
}

LLINT32 
LLog::start() {
    return LLog::Service::getIns()->exec();
}

LLINT32
LLog::terminal() {
    return LLog::Service::getIns()->terminal();
}

