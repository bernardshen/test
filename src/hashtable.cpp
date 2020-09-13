#include "hashtable.hpp"

HashTable::HashTable(char *buffer, uint64_t count) {
    if (buffer == NULL) {
        Debug::notifyError("HashTable: buffer is null");
        exit(EXIT_FAILURE);
    }
    itemsHash = (HashItem *)buffer;
    bitmapChainedItems = new Bitmap(count, buffer + HASH_ITEMS_SIZE);
    itemsChained = (ChainedItem *)(buffer + HASH_ITEMS_SIZE + count / 8);
    if (bitmapChainedItems->set(0) == false) {
        Debug::notifyError("HashTable: bitmap set error.\n");
        exit(EXIT_FAILURE);
    }
    sizeBufferUsed = HASH_ITEMS_SIZE + count / 8 + sizeof(ChainedItem) * count;
    headFreeBit = NULL;
    FreeBit *currentFreeBit;
    for (uint64_t i = 1; i < bitmapChainedItems->countTotal(); i++) {
        currentFreeBit = (FreeBit *)malloc(sizeof(FreeBit));
        currentFreeBit->pos = i;
        currentFreeBit->next = headFreeBit;
        headFreeBit = currentFreeBit;
    }
}

HashTable::~HashTable() {
    FreeBit *currentFreeBit;
    while (headFreeBit != NULL) {
        currentFreeBit = headFreeBit;
        headFreeBit = (FreeBit *)(headFreeBit->next);
        free(currentFreeBit);
    }
    delete bitmapChainedItems;
}

void HashTable::getAddressHash(const char *buf, uint64_t len, AddressHash *hashAddress) {
    uint64_t sha256_buf[4];

    SHA256 sha256;
    sha256.CalculateDigest((byte *)sha256_buf, (const byte *)buf, len);
    *hashAddress = sha256_buf[0] & 0x00000000000FFFFF;
}

AddressHash HashTable::getAddressHash(UniqueHash *hashUnique) {
    return hashUnique->value[0] & 0x00000000000FFFFF;
}

void HashTable::getUniqueHash(const char *buf, uint64_t len, UniqueHash *hashUnique) {
    SHA256 sha256;
    sha256.CalculateDigest((byte *)hashUnique, (const byte *)buf, len);
}

bool HashTable::get(const char *path, uint64_t *indexMeta, bool *isDirectory) {
    if ((path == NULL) || (indexMeta == NULL) || (isDirectory == NULL)) {
        return false;
    }
    UniqueHash hashUnique;
    HashTable::getUniqueHash(path, strlen(path), &hashUnique);
    AddressHash hashAddress = HashTable::getAddressHash(&hashUnique);
    bool result;
    mutexBitmapChainedItems.lock();
    {
        uint64_t indexHead = itemsHash[hashAddress].indexHead;
        if (indexHead == 0) {
            result = false;
        } else {
            uint64_t indexCurrent = indexHead;
            bool found = false;
            do {
                if (memcmp(&(itemsChained[indexCurrent].hashUnique), &(hashUnique), sizeof(UniqueHash)) == 0) {
                    *indexMeta = itemsChained[indexCurrent].indexMeta;
                    *isDirectory = itemsChained[indexCurrent].isDirectory;
                    found = true;
                    break;
                } else {
                    indexCurrent = itemsChained[indexCurrent].indexNext;
                }
            } while (indexCurrent != 0);
            result = found;
        }
    }
    mutexBitmapChainedItems.unlock();
    return result;
}

bool HashTable::get(UniqueHash *hashUnique, uint64_t *indexMeta, bool *isDirectory) {
    if ((hashUnique == NULL) || (indexMeta == NULL) || (isDirectory == NULL)) {
        return false;
    }
    AddressHash hashAddress = HashTable::getAddressHash(hashUnique);
    bool result = false;
    mutexBitmapChainedItems.lock();
    {
        uint64_t indexHead = itemsHash[hashAddress].indexHead;
        if (indexHead == 0) {
            result = false;
        } else {
            uint64_t indexCurrent = indexHead;
            bool found = false;
            do {
                if (memcmp(&(itemsChained[indexCurrent].hashUnique), hashUnique, sizeof(UniqueHash)) == 0) {
                    *indexMeta = itemsChained[indexCurrent].indexMeta;
                    *isDirectory = itemsChained[indexCurrent].isDirectory;
                    found = true;
                    break;
                } else {
                    indexCurrent = itemsChained[indexCurrent].indexNext;
                }
            } while (indexCurrent != 0);
            result = found;
        }
    }
    mutexBitmapChainedItems.unlock();
    return result;
}

bool HashTable::put(const char *path, uint64_t indexMeta, bool isDirectory) {
    if (path == NULL) {
        return false;
    }
    AddressHash hashAddress;
    HashTable::getAddressHash(path, strlen(path), &hashAddress);
    bool result;
    mutexBitmapChainedItems.lock();
    {
        uint64_t indexHead = itemsHash[hashAddress].indexHead;
        if (indexHead == 0) {
            uint64_t index;
            if (headFreeBit == NULL) {
                result = false;
            } else {
                index = headFreeBit->pos;
                FreeBit *currentFreeBit = headFreeBit;
                headFreeBit = (FreeBit *)headFreeBit->next;
                free(currentFreeBit);
                if (bitmapChainedItems->set(index) == false) {
                    result = false;
                } else {
                    itemsChained[index].indexNext = 0;
                    itemsChained[index].indexMeta = indexMeta;
                    itemsChained[index].isDirectory = isdigit;
                    UniqueHash hashUnique;
                    HashTable::getUniqueHash(path, strlen(path), &hashUnique);
                    itemsChained[index].hashUnique = hashUnique;
                    itemsHash[hashAddress].indexHead = index;
                    result = true;
                }
            }
        } else {
            UniqueHash hashUnique;
            getUniqueHash(path, strlen(path), &hashUnique);
            uint64_t indexCurrent = indexHead;
            uint64_t indexBeforeCurrent = 0;
            bool found = false;
            do {
                if (memcmp(&(itemsChained[indexCurrent].hashUnique), &hashUnique, sizeof(UniqueHash)) == 0) {
                    itemsChained[indexCurrent].indexMeta = indexMeta;
                    itemsChained[indexCurrent].isDirectory = isDirectory;
                    found = true;
                    break;
                } else {
                    indexBeforeCurrent = indexCurrent;
                    indexCurrent = itemsChained[indexCurrent].indexNext;
                }
            } while (indexCurrent != 0);
            if (found == true) {
                result = true;
            } else {
                uint64_t index;
                if (headFreeBit == NULL) {
                    result = false;
                } else {
                    index = headFreeBit->pos;
                    FreeBit *currentFreeBit = headFreeBit;
                    headFreeBit = (FreeBit *)headFreeBit->next;
                    free(currentFreeBit);
                    if (bitmapChainedItems->set(index) == false) {
                        result = false;
                    } else {
                        itemsChained[index].indexNext = 0;
                        itemsChained[index].indexMeta = indexMeta;
                        itemsChained[index].isDirectory = isDirectory;
                        UniqueHash hashUnique;
                        HashTable::getUniqueHash(path, strlen(path), &hashUnique);
                        itemsChained[index].hashUnique = hashUnique;
                        itemsChained[indexBeforeCurrent].indexNext = index;
                        result = true;
                    }
                }
            }
        }
    }
    mutexBitmapChainedItems.unlock();
    return result;
}

bool HashTable::put(UniqueHash *hashUnique, uint64_t indexMeta, bool isDirectory) {
    if (hashUnique == NULL) {
        return false;
    }
    AddressHash hashAddress = HashTable::getAddressHash(hashUnique);
    bool result = false;
    mutexBitmapChainedItems.lock();
    {
        uint64_t indexHead = itemsHash[hashAddress].indexHead;
        if (indexHead == 0) {
            uint64_t index;
            if (headFreeBit == NULL) {
                result = false;
            } else {
                index = headFreeBit->pos;
                FreeBit *currentFreeBit = headFreeBit;
                headFreeBit = (FreeBit *)(headFreeBit->next);
                free(currentFreeBit);
                if (bitmapChainedItems->set(index) == false) {
                    result = false;
                } else {
                    itemsChained[index].indexNext = 0;
                    itemsChained[index].indexMeta = indexMeta;
                    itemsChained[index].isDirectory = isDirectory;
                    itemsChained[index].hashUnique = *hashUnique;
                    itemsHash[hashAddress].indexHead = index;
                    result = true;
                }
            }
        } else {
            uint64_t indexCurrent = indexHead;
            uint64_t indexBeforeCurrent = 0;
            bool found = false;
            do {
                if (memcmp(&(itemsChained[indexCurrent].hashUnique), hashUnique, sizeof(UniqueHash)) == 0) {
                    itemsChained[indexCurrent].indexMeta = indexMeta;
                    itemsChained[indexCurrent].isDirectory = isDirectory;
                    found = true;
                    break;
                } else {
                    indexBeforeCurrent = indexCurrent;
                    indexCurrent = itemsChained[indexCurrent].indexNext;
                }
            } while (indexCurrent != 0);
            if (found) {
                result = true;
            } else {
                uint64_t index;
                if (headFreeBit == NULL) {
                    result = false;
                } else {
                    index = headFreeBit->pos;
                    FreeBit *currentFreeBit = headFreeBit;
                    headFreeBit = (FreeBit *)(headFreeBit->next);
                    free(currentFreeBit);
                    if (bitmapChainedItems->set(index) == false) {
                        result = false;
                    } else {
                        itemsChained[index].indexNext = 0;
                        itemsChained[index].indexMeta = indexMeta;
                        itemsChained[index].isDirectory = isDirectory;
                        itemsChained[index].hashUnique = *hashUnique;
                        itemsChained[indexBeforeCurrent].indexNext = index;
                        result = true;
                    }
                }
            }
        }
    }
    mutexBitmapChainedItems.unlock();
    return result;
}

bool HashTable::del(const char *path) {
    if (path == NULL) {
        return false;
    }
    AddressHash hashAddress;
    HashTable::getAddressHash(path, strlen(path), &hashAddress);
    bool result;
    mutexBitmapChainedItems.lock();
    {
        uint64_t indexHead = itemsHash[hashAddress].indexHead;
        if (indexHead == 0) {
            result = false;
        } else {
            UniqueHash hashUnique;
            HashTable::getUniqueHash(path, strlen(path), &hashUnique);
            if (memcmp(&(itemsChained[indexHead].hashUnique), &hashUnique, sizeof(UniqueHash)) == 0) {
                if (bitmapChainedItems->clear(indexHead) == false) {
                    result = false;
                } else {
                    FreeBit *currentFreeBit = (FreeBit *)malloc(sizeof(FreeBit));
                    currentFreeBit->next = headFreeBit;
                    currentFreeBit->pos = indexHead;
                    headFreeBit = currentFreeBit;
                    itemsHash[hashAddress].indexHead = itemsChained[indexHead].indexNext;
                    result = true;
                }
            } else {
                uint64_t indexBeforeCurrent = indexHead;
                uint64_t indexCurrent = itemsChained[indexHead].indexNext;
                bool found = false;
                while (indexCurrent != 0) {
                    if (memcmp(&(itemsChained[indexCurrent].hashUnique), &hashUnique, sizeof(UniqueHash)) == 0) {
                        found = true;
                        break;
                    } else {
                        indexBeforeCurrent = indexCurrent;
                        indexCurrent = itemsChained[indexCurrent].indexNext;
                    }
                }
                if (found == false) {
                    result = true;
                } else {
                    if (bitmapChainedItems->clear(indexCurrent) == false) {
                        result = false;
                    } else {
                        FreeBit *currentFreeBit = (FreeBit *)malloc(sizeof(FreeBit));
                        currentFreeBit->pos = indexCurrent;
                        currentFreeBit->next = headFreeBit;
                        headFreeBit = currentFreeBit;
                        itemsChained[indexBeforeCurrent].indexNext = itemsChained[indexCurrent].indexNext;
                        result = true;
                    }
                }
            }
        }
    }
    mutexBitmapChainedItems.unlock();
    return result;
}

bool HashTable::del(UniqueHash *hashUnique) {
    if (hashUnique == NULL) {
        return false;
    }
    AddressHash hashAddress = HashTable::getAddressHash(hashUnique);
    bool result;
    mutexBitmapChainedItems.lock();
    {
        uint64_t indexHead = itemsHash[hashAddress].indexHead;
        if (indexHead == 0) {
            result = false;
        } else {
            if (memcmp(&(itemsChained[indexHead].hashUnique), hashUnique, sizeof(UniqueHash)) == 0) {
                if (bitmapChainedItems->clear(indexHead) == false) {
                    result = false;
                } else {
                    FreeBit *currentFreeBit = (FreeBit *)malloc(sizeof(currentFreeBit));
                    currentFreeBit->next = headFreeBit;
                    currentFreeBit->pos = indexHead;
                    headFreeBit = currentFreeBit;
                    itemsHash[hashAddress].indexHead = itemsChained[indexHead].indexNext;
                    result = true;
                }
            } else {
                uint64_t indexBeforeCurrent = indexHead;
                uint64_t indexCurrent = itemsChained[indexHead].indexNext;
                bool found = false;
                while (indexCurrent != 0) {
                    if (memcmp(&(itemsChained[indexCurrent].hashUnique), hashUnique, sizeof(UniqueHash)) == 0) {
                        found = true;
                        break;
                    } else {
                        indexBeforeCurrent = indexCurrent;
                        indexCurrent = itemsChained[indexCurrent].indexNext;
                    }
                }
                if (found == false) {
                    result = true;
                } else {
                    if (bitmapChainedItems->clear(indexBeforeCurrent) == false) {
                        result = false;
                    } else {
                        FreeBit *currentFreeBit = (FreeBit *)malloc(sizeof(FreeBit));
                        currentFreeBit->pos = indexCurrent;
                        currentFreeBit->next = headFreeBit;
                        headFreeBit = currentFreeBit;
                        itemsChained[indexBeforeCurrent].indexNext = itemsChained[indexCurrent].indexNext;
                        result = true;
                    }
                }
            }
        }
    }
    mutexBitmapChainedItems.unlock();
    return result;
}

uint64_t HashTable::getSavedHashItemsCount() {
    uint64_t count = 0;
    for (uint64_t i = 0; i < HASH_ITEMS_COUNT; i++) {
        if (itemsHash[i].indexHead != 0) {
            count ++;
        }
    }
    return count;
}

uint64_t HashTable::getSavedChainedItemsCount() {
    return (bitmapChainedItems->countTotal() - bitmapChainedItems->countFree() - 1);
}

uint64_t HashTable::getTotalHashItemsCount() {
    return HASH_ITEMS_COUNT;
}

uint64_t HashTable::getTotalChainedItemsCount() {
    return (bitmapChainedItems->countTotal() - 1);
}

uint64_t HashTable::getMaxLengthOfChain() {
    uint64_t max = 0, length;
    for (uint64_t i = 0; i < HASH_ITEMS_COUNT; i++) {
        length = 0;
        if (itemsHash[i].indexHead != 0) {
            uint64_t indexCurrent = itemsHash[i].indexHead;
            do {
                length ++;
                indexCurrent = itemsChained[indexCurrent].indexNext;
            } while (indexCurrent != 0);
            if (length > max) 
                max = length;
        }
    }
    return max;
}