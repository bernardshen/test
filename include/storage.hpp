#ifndef STORAGE_HEADER
#define STORAGE_HEADER

#include <stdint.h>
#include "hashtable.hpp"
#include "table.hpp"
#include "global.h"

typedef struct {
    char bytes[BLOCK_SIZE];
} Block;

class Storage {
private:
    uint64_t countNode;

public:
    uint64_t sizeBufferUsed;
    HashTable *hashtable;
    Table<FileMeta> *tableFileMeta;
    Table<DirectoryMeta> *tableDirectoryMeta;
    Table<Block> *tableBlock;
    NodeHash getNodeHash(UniqueHash *hashUnique);
    Storage(char *buffer, char *bufferBlock, uint64_t countFile, uint64_t countDirectory, uint64_t countBlock, uint64_t countNode);
    ~Storage();
};

#endif