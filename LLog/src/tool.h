#ifndef TOOL_H
#define TOOL_H

SPACE_BEGIN(Tool)
    LLBOOL  check_binary(LLINT64 _num);
    LLINT64 get_minimum_greater(LLINT64 _num);
    void    format_timestamp(LLCHAR* _buffer, LUINT64 _timestamp);
    LUINT32 num2string(LLINT64 _num, LLCHAR* _buffer, LUINT32 _min_size = 0);
    LUINT32 num2string(LUINT64 _num, LLCHAR* _buffer, LUINT32 _min_size = 0);
SPACE_END()

#endif // !TOOL_H
