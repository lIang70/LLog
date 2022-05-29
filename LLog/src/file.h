#ifndef FILE_H
#define FILE_H

#include "global.h"

LLOG_SPACE_BEGIN()
    class Buffer;
LLOG_SPACE_END()


class File {
    static LLINT32 s_nPageSize;

    LLCHAR  m_cCurPath[FILEPATHLEN];
    LLCHAR  m_cCurFile[FILELEN];
    LUINT32 m_nFileIndex = -1;
    LUINT32 m_nCurLen = 0;
    LUINT32 m_nMaxSize = (12 * 1024 * 1024);

    LLINT32 m_nFd = -1;
    LLCHAR  *m_pMapData = nullptr;

private:
    bool checkPath(LLCHAR* _path);
    void rollFile();

public:
    File ();
    ~File ();

    void setFilePath(LLCHAR* _path);
    void setFileName(LLCHAR* _filename);
    void setFileMaxSize(LUINT32 _size);
    void writeBuffer(LLog::Buffer *_buffer);
};

#endif // !FILE_H