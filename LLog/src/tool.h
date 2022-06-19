#ifndef TOOL_H
#define TOOL_H

SPACE_BEGIN(Tool)
        LLBOOL  check_binary(LLINT64 _num);
        LLINT64 get_minimum_greater(LLINT64 _num);

        LUINT64 get_system_time();
        LUINT32 get_tid();

        void    init_system_info();
const   LUINT32 get_pid();
const   LLCHAR* get_hostname();

        /**
         * @brief Format the timestamp, as ${%4Y-%2M-%2D %2H:%2M:%2S.%6ns}, it's length is 26.
         * 
         * @param _buffer       The target cache.
         * @param _timestamp    The time stamp. 
         */
        void    format_timestamp(LLCHAR* _buffer, LUINT64 _timestamp);
        LUINT32 num2string(LLINT64 _num, LLCHAR* _buffer, LUINT32 _min_size = 0);
        LUINT32 num2string(LUINT64 _num, LLCHAR* _buffer, LUINT32 _min_size = 0);
SPACE_END()

#endif // !TOOL_H
