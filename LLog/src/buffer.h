#ifndef LLOG_BUFFER_H
#define LLOG_BUFFER_H

#include "global.h"

LLOG_SPACE_BEGIN()
    class buffer_base {
    public:
        static constexpr LUINT32	m_nFixedSize = 256 - sizeof(LUINT32) * 2 - sizeof(std::unique_ptr<char[]>) - 8;

    private:
        std::unique_ptr<char[]>	    m_uHeapBuffer;
        LLCHAR						m_cStackBuffer[m_nFixedSize];
        LUINT32						m_nBufferCap = 0;
        LUINT32						m_nBufferI = 0;

    public:
        explicit buffer_base(LUINT32 _size = 0);
        buffer_base(const buffer_base& _another) noexcept;
        buffer_base(buffer_base&& _another) noexcept;
        virtual ~buffer_base() = default;

        virtual buffer_base& operator=(const buffer_base& _another) noexcept;
        virtual buffer_base& operator=(buffer_base&& _another) noexcept;

        virtual LUINT32 index_add(LUINT32 _size = 0);
        virtual LLCHAR* buffer_begin();
        virtual LUINT32 cap() const;
        virtual LLBOOL	is_needed_resize(LUINT32 _size);
        virtual void	swap(buffer_base& _buffer);


    };

    class Stream : public buffer_base {
    private:
        void stringify(LLCHAR*& _buffer, LUINT32& _index, LLCHAR* _start, const LLCHAR* const _end);

    public:
        explicit Stream(LUINT32 _size = 0);
        ~Stream();
        
        LLCHAR* buffer_begin() override;
        LUINT32 index_add(LUINT32 _size = 0) override;
        
        void operator<<(LLCHAR _val);
        void operator<<(LLCHAR* _val);
        void operator<<(const LLCHAR* _val);
        void operator<<(LLINT16 _val);
        void operator<<(LUINT16 _val);
        void operator<<(LLINT32 _val);
        void operator<<(LUINT32 _val);
        void operator<<(LLINT64 _val);
        void operator<<(LUINT64 _val);
        void operator<<(LLFLOAT _val);
        void operator<<(LLDOUBLE _val);
        
        template<typename Type>
        inline void encode(Type _val) {
            *reinterpret_cast<Type*>(&buffer_begin()[index_add(sizeof(Type))]) = _val;
        }
        template<typename Type>
        inline void encode(Type _val, LUINT8 _id) {
            is_needed_resize(sizeof(Type) + sizeof(LUINT8));
            encode<LUINT8>(_id);
            encode<Type>(_val);
        }
        
        void encodeString(const LLCHAR* _val);
        void decode2Buffer(LLCHAR* __buffer, LUINT32 & __size);
	};

LLOG_SPACE_END()


#endif // !LLOG_BUFFER_H
