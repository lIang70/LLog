#ifndef LLOG_BUFFER_H
#define LLOG_BUFFER_H

#include "global.h"

LLOG_SPACE_BEGIN()
    class buffer_base {
    public:
        static constexpr LUINT32	m_nFixedSize = 256 - sizeof(LUINT32) * 2 - sizeof(std::unique_ptr<char[]>) - 8;

    private:
        LLCHAR						m_cStackBuffer[m_nFixedSize];
        std::unique_ptr<char[]>	    m_uHeapBuffer;
        LUINT32						m_nBufferCap = 0;
        LUINT32						m_nBufferI = 0;

    public:
        explicit    buffer_base(LUINT32 _size = 0);
                    buffer_base(const buffer_base& _another) noexcept;
                    buffer_base(buffer_base&& _another) noexcept;
        virtual     ~buffer_base() = default;

        virtual     buffer_base& operator=(const buffer_base& _another) noexcept;
        virtual     buffer_base& operator=(buffer_base&& _another) noexcept;

        virtual     void    reset();
        virtual     LUINT32 index_add(LUINT32 _size = 0);
        virtual     LLCHAR* buffer_begin();
                    LUINT32 cap() const;
                    LLBOOL	is_needed_resize(LUINT32 _size);
                    void	swap(buffer_base& _buffer);

    };

    class Stream : public buffer_base {
    public:
        explicit Stream(LUINT32 _size = 0);
        ~Stream();
        
        LLCHAR* buffer_begin() override;
        LUINT32 index_add(LUINT32 _size = 0) override;
        
        Stream & operator<<(LLCHAR _val);
        Stream & operator<<(LLCHAR* _val);
        Stream & operator<<(const LLCHAR* _val);
        Stream & operator<<(LLINT16 _val);
        Stream & operator<<(LUINT16 _val);
        Stream & operator<<(LLINT32 _val);
        Stream & operator<<(LUINT32 _val);
        Stream & operator<<(LLINT64 _val);
        Stream & operator<<(LUINT64 _val);
        Stream & operator<<(LLFLOAT _val);
        Stream & operator<<(LLDOUBLE _val);
	};

LLOG_SPACE_END()


#endif // !LLOG_BUFFER_H
