
#ifndef LOAD_LIB_H
#define LOAD_LIB_H

#include "Define.h"

#define FILE_FORMAT_VERSION 18

union u_map_fcc {
    char fcc_txt[4];
    uint32 fcc;
};

//
// File version chunk
//
struct file_MVER
{
    union {
        uint32 fcc;
        char fcc_txt[4];
    };
    uint32 size;
    uint32 ver;
};

class FileLoader
{
    uint8* data;
    uint32 data_size;

public:
    virtual bool prepareLoadedData();
    uint8* GetData() { return data; }
    uint32 GetDataSize() { return data_size; }

    file_MVER* version;
    FileLoader();
    ~FileLoader();
    bool loadFile(char* filename, bool log = true);
    virtual void free();
};
#endif
