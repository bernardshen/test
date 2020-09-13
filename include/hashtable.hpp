#ifndef HASHTABLE_HEADER
#define HASHTABLE_HEADER

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mutex>
#include "bitmap.hpp"
#include "debug.hpp"
#include "table.hpp"
#include <cryptopp/sha.h>

using namespace CryptoPP;

#define HASH_ADDRESS_BITS 20
#define HASH_ITEMS_COUNT (1 << HASH_ADDRESS_BITS)
#define HASH_ITEMS_SIZE (HASH_ITEMS_COUNT * sizeof(HashItem))

typedef uint64_t AddressHash;

typedef struct {
    uint64_t value[4];
} UniqueHash;

typedef struct {
    uint64_t key;
    uint64_t indexHead;
} HashItem;

typedef struct {
    uint64_t indexNext;
    uint64_t indexMeta;
    bool isDirectory;
    UniqueHash hashUnique;
} ChainedItem;

class HashTable {
private:
    Bitmap *bitmapChainedItems;
    std::mutex mutexBitmapChainedItems;
    HashItem *itemsHash;
    ChainedItem *itemsChained;
    FreeBit *headFreeBit;

public:
    static void getAddressHash(const char *buf, uint64_t len, AddressHash *hashAddress);
    static AddressHash getAddressHash(UniqueHash *hasUnique);
    static void getUniqueHash(const char *buf, uint64_t len, UniqueHash *hashUnique);
    uint64_t sizeBufferUsed;
    bool get(const char *path, uint64_t *indexMeta, bool *isDirectory);
    bool get(UniqueHash *hashUnique, uint64_t *indexMeta, bool *isDirectory);
    bool put(const char *path, uint64_t indexMeta, bool isDirectory);
    bool put(UniqueHash *hashUnique, uint64_t indexMeta, bool isDirectory);
    bool del(const char *path);
    bool del(UniqueHash *hashUnique);
    uint64_t getSavedHashItemsCount();
    uint64_t getSavedChainedItemsCount();
    uint64_t getTotalHashItemsCount();
    uint64_t getTotalChainedItemsCount();
    uint64_t getMaxLengthOfChain();
    HashTable(char *buffer, uint64_t count);
    ~HashTable();
};

#endif