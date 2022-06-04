#include "file.h"
#include "buffer.h"

LLINT32 File::s_nPageSize = -1;

bool 
File::checkPath(LLCHAR* _path) {
    return access(_path, 0) == F_OK;
}

void 
File::rollFile() {
    if (!checkPath(m_cCurPath)) {
        LLINT32 isCreate = mkdir(m_cCurPath, S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
        if (isCreate)
            printf("[LLog error] rollFile() create file failure!\n");
    }
    LLCHAR _wholepath[FILEPATHLEN + HOSTNAMELEN];
    if (++m_nFileIndex) {
        sprintf(_wholepath, "%s%s.%d.log", m_cCurPath, m_cCurFile, m_nFileIndex);
    } else {
        sprintf(_wholepath, "%s%s.log", m_cCurPath, m_cCurFile);
    }

    m_nFd = open(_wholepath, O_RDWR | O_CREAT, S_IRWXU);
    lseek(m_nFd, m_nMaxSize - 1, SEEK_END);
    if (write(m_nFd, "", 1) == -1) {
        printf("[LLog error] File::rollFile() write failed :%m!\n");
    }
    m_nCurLen = 0;
    auto _p = mmap(0, m_nMaxSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_nFd, 0);
    if (_p == MAP_FAILED) {
        printf("[LLog error] File::rollFile() apping failed :%m!\n");
    } else {
        m_pMapData = static_cast<LLCHAR*>(_p);
    }

}

File::File() {
    memset(m_cCurPath, 0, FILEPATHLEN * sizeof(LLCHAR));
#ifdef C_OS_WIN
    // todo
#elif defined(C_OS_LINUX)
    if (getcwd(m_cCurPath, FILEPATHLEN) == NULL) {
        printf("[LLog error] File::constructor() getcwd failed :%m!\n");
    }
    LLCHAR* ch = m_cCurPath;
    while (*++ch != '\0');
    *ch++ = '/';
    *ch++ = 'l';
    *ch++ = 'o';
    *ch++ = 'g';
    *ch++ = '/';

    if (s_nPageSize == -1) 
        s_nPageSize = getpagesize();
#endif // C_OS_WIN
    
}

File::~File() {
    if (m_nFd != -1) {
        munmap(m_pMapData, m_nMaxSize);
        close(m_nFd);
        m_nFd = -1;
    }
}

void 
File::setFilePath(LLCHAR* _path) {
    memcpy(m_cCurPath, _path, FILEPATHLEN);
    LLCHAR* ch = m_cCurPath;
    while (*++ch != '\0');
    if (*--ch != '/')
        *++ch = '/';
}

void 
File::setFileName(LLCHAR* _filename) {
    memcpy(m_cCurFile, _filename, FILELEN);
}

void 
File::setFileMaxSize(LUINT32 _size) {
    m_nMaxSize = _size;
}

void 
File::writeBuffer(LLCHAR* _buffer, LUINT32 _size) {
    if (m_nFd == -1) {
        rollFile();
    }
#ifdef C_OS_WIN
    // todo
#elif defined(C_OS_LINUX)
    m_nCurLen += _size;
    LUINT32 _r_size = _size;
    while (_size <= _r_size) {
        memcpy(m_pMapData, _buffer, LMIN(_size, s_nPageSize));
        m_pMapData += LMIN(_size, s_nPageSize);
        _buffer += LMIN(_size, s_nPageSize);
        _size -= s_nPageSize;
    }
    if (m_nCurLen + _r_size >= m_nMaxSize) {
        munmap(m_pMapData, m_nMaxSize);
        close(m_nFd);
        m_nFd = -1;
    }
#endif // C_OS_WIN
}