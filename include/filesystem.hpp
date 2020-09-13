#ifndef FILESYSTEM_HEADER
#define FILESYSTEM_HEADER

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <mutex>
#include "storage.hpp"
#include "debug.hpp"
#include "global.h"
// #include "lock.h"

class FileSystem {
private:
    Storage *storage;
    NodeHash hashLocalNode;
    // LockService *lock;
    uint64_t addressHashTable;
    bool checkLocal(NodeHash hashNode);
    bool getParentDirectory(const char *path, char *parent);
    bool getNameFromPath(const char *path, char *name);
    bool sendMessage(NodeHash hashNode, void *bufferSend, uint64_t lenghtSend,
                     void *bufferReceive, uint64_t lengthReceive);
    bool fillFilePositionInformation(uint64_t size, uint64_t offset, file_pos_info *fpi, FileMeta *metaFile);

public:
    // a file system operation
    void rootInitialize(NodeHash localNode);
    bool mknod(const char *path);
    bool mknod2pc(const char *path);
    bool mknodcd(const char *path);
    bool getattr(const char *path, FileMeta *attribute);
    bool access(const char *path);
    bool mkdir(const char *path);
    bool mkdir2pc(const char *path);
    bool mkdircd(const char *path);
    bool readdir(const char *path, nrfsfilelist *list);
    
};



#endif