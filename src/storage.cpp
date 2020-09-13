#include "storage.hpp"

Storage::Storage(char *buffer, char *bufferBlock, uint64_t countFile, uint64_t countDirectory, uint64_t countBlock, uint64_t countNode) {
    if ((buffer == NULL) || (bufferBlock == NULL) || (countFile == 0) || (countDirectory == 0) || 
        (countBlock == 0) || (countNode == 0)) {
            Debug::notifyError("Storage: parameter error.\n");
            exit(EXIT_FAILURE);
    }
    hashtable = new HashTable(buffer, countDirectory + countFile);
    Debug::notifyInfo("sizeof Hash Table = %d MB", hashtable->sizeBufferUsed / 1024 / 1024);

    tableFileMeta = new Table<FileMeta>(buffer + hashtable->sizeBufferUsed, countFile);
    Debug::notifyInfo("sizeof File Meta Size = %d MB", tableFileMeta->sizeBufferUsed / 1024 / 1024);

    tableDirectoryMeta = new Table<DirectoryMeta>(buffer + hashtable->sizeBufferUsed + tableFileMeta->sizeBufferUsed, countDirectory);
    Debug::notifyInfo("sizeof Directory Meta Size = %d MB", tableDirectoryMeta->sizeBufferUsed / 1024 / 1024);

    tableBlock = new Table<Block>(bufferBlock, countBlock);

    this->countNode = countNode;
    sizeBufferUsed = hashtable->sizeBufferUsed + tableFileMeta->sizeBufferUsed + tableDirectoryMeta->sizeBufferUsed + tableBlock->sizeBufferUsed;
}

Storage::~Storage() {
    delete hashtable;
    delete tableFileMeta;
    delete tableDirectoryMeta;
    delete tableBlock;
}

NodeHash Storage::getNodeHash(UniqueHash *hashUnique) {
    if (hashUnique == NULL) {
        Debug::notifyError("Storage::getNodeHash: buffer is null.\n");
        exit(EXIT_FAILURE);
    }
    return ((hashUnique->value[3] % countNode) + 1);
}