#ifndef TOOL_H
#define TOOL_H

#include "global.h"

SPACE_BEGIN(Tool)
    void    format_timestamp(LLCHAR* _buffer, LUINT64 _timestamp);
    LUINT32 num2string(LLINT64 _num, LLCHAR* _buffer, LUINT32 _min_size = 0);
    LUINT32 num2string(LUINT64 _num, LLCHAR* _buffer, LUINT32 _min_size = 0);
SPACE_END()

#endif // !TOOL_H
