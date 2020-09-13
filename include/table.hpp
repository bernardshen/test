#ifndef TABLE_HEADER
#define TABLE_HEADER

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mutex>
#include "bitmap.hpp"
#include "debug.hpp"

typedef struct {
    uint64_t pos;
    void *next;
} FreeBit;

template<typename T> class Table {
private:
    Bitmap *bitmapItems;
    std::mutex mutexBitmapItems;
    T *items;
    FreeBit *headFreeBit;
    FreeBit *tailFreeBit;

public:
    uint64_t sizeBufferUsed;
    bool create(uint64_t *index, T *item);
    bool create(uint64_t *index);
    bool get(uint64_t index, T *item);
    bool get(uint64_t index, T *item, uint64_t *address);
    bool put(uint64_t index, T *item);
    bool put(uint64_t index, T *item, uint64_t *address);
    bool remove(uint64_t index);
    uint64_t countSavedItems();
    uint64_t countTotalItems();
    Table(char *buffer, uint64_t count);
    ~Table();
};

template<typename T> Table<T>::Table(char *buffer, uint64_t count) {
    if (buffer == NULL) {
        Debug::notifyError("Table: buffer is null.\n");
        exit(EXIT_FAILURE);
    }
    if (count % 8 != 0) {
        Debug::notifyError("Table: count should be times of 8\n");
        exit(EXIT_FAILURE);
    }
    bitmapItems = new Bitmap(count, buffer + count * sizeof(T));
    items = (T*)(buffer);
    sizeBufferUsed = count / 8 + count * sizeof(T);
    
    // initialize free bit chain
    headFreeBit = NULL;
    FreeBit * currentFreeBit;
    for (int index = count / 8 - 1; index >= 0; index --) {
        for (int offset = 7; offset >= 0; offset --) {
            if ((buffer[index] & (1 << (7 - offset))) == 0) {
                currentFreeBit = (FreeBit *)malloc(sizeof(FreeBit));
                currentFreeBit->pos = index * 8 + offset;
                currentFreeBit->next = headFreeBit;
                headFreeBit = currentFreeBit;
            }
        }
    }
}

template<typename T> Table<T>::~Table() {
    FreeBit *currentFreeBit;
    while (headFreeBit != NULL) {
        currentFreeBit = headFreeBit;
        headFreeBit = (FreeBit *)(headFreeBit->next);
        free(currentFreeBit);
    }
    delete bitmapItems;
}

template<typename T> bool Table<T>::create(uint64_t *index) {
    if (index == NULL) {
        return false;
    }
    bool result = false;
    mutexBitmapItems.lock();
    {
        if (headFreeBit == NULL) {
            result = false;
        } else {
            *index = headFreeBit->pos;
            FreeBit *currentFreeBit = headFreeBit;
            headFreeBit = (FreeBit *)(headFreeBit->next);
            free(currentFreeBit);
            if (bitmapItems->set(*index) == false) {
                result = false;
            } else {
                result = true;
            }
            
        }
    }
    mutexBitmapItems.unlock();
    return result;
}

template<typename T> bool Table<T>::create(uint64_t *index, T *item) {
    if (item == NULL) {
        return false;
    }
    bool result;
    mutexBitmapItems.lock();
    {
        if (headFreeBit == NULL) {
            return false;
        } else {
            *index = headFreeBit->pos;
            FreeBit *currentFreeBit = headFreeBit;
            headFreeBit = (FreeBit *)(headFreeBit->next);
            free(currentFreeBit);
            if (bitmapItems->set(*index) == false) {
                result = false;
            } else {
                items[*index] = *item;
                result = true;
            }
        }
    }
    mutexBitmapItems.unlock();
    return result;
}

template<typename T> bool Table<T>::get(uint64_t index, T *item) {
    if (item == NULL) {
        return false;
    }
    bool result = false;
    mutexBitmapItems.lock();
    {
        bool status;
        if (bitmapItems->get(index, &status) == false)
            result = false;
        else {
            if (status == false)
                result = false;
            else {
                *item = items[index];
                result = true;
            }
        }
    }
    mutexBitmapItems.unlock();
    return result;
}

template<typename T> bool Table<T>::get(uint64_t index, T *item, uint64_t *address) {
    if (item == NULL) {
        return false;
    }
    bool result = false;
    mutexBitmapItems.lock();
    {
        bool status;
        if (bitmapItems->get(index, &status) == false) {
            result = false;
        } else {
            if (status == false) {
                result = false;
            } else {
                *item = items[index];
                *address = (uint64_t)(&items[index]);
                Debug::debugItem("get, address = %lx", *address);
                result = true;
            }
        }
    }
    mutexBitmapItems.unlock();
    return result;
}

template<typename T> bool Table<T>::put(uint64_t index, T *item) {
    if (item == NULL) {
        return false;
    }
    bool result = false;
    mutexBitmapItems.lock();
    {
        bool status;
        if (bitmapItems->get(index, &status) == false) {
            result = false;
        } else {
            if (status == false) {
                result = false;
            } else {
                items[index] = *item;
                result = true;
            }
        }
    }
    mutexBitmapItems.unlock();
    return result;
}

template<typename T> bool Table<T>::put(uint64_t index, T *item, uint64_t *address) {
    bool result = false;
    mutexBitmapItems.lock();
    {
        bool status;
        if (bitmapItems->get(index, &status) == false) {
            result = false;
        } else {
            if (status == false) {
                result = false;
            } else {
                items[index] = *item;
                *address = (uint64_t)(&items[index]);
                Debug::debugItem("put, address = %lx", *address);
                result = true;
            }
        }
    }
    mutexBitmapItems.unlock();
    return result;
}

template<typename T> bool Table<T>::remove(uint64_t index) {
    bool result;
    mutexBitmapItems.lock();
    {
        bool status;
        if (bitmapItems->get(index, &status) == false) {
            result = false;
        } else {
            if (status == false) {
                result = false;
            } else {
                if (bitmapItems->clear(index) == false) {
                    result = false;
                } else {
                    FreeBit *currentFreeBit = (FreeBit *)malloc(sizeof(FreeBit));
                    currentFreeBit->pos = index;
                    currentFreeBit->next = headFreeBit;
                    headFreeBit = currentFreeBit;
                    result = true;
                }
            }
        }
    }
    mutexBitmapItems.unlock();
    return result;
}

template<typename T> uint64_t Table<T>::countSavedItems() {
    return (bitmapItems->countTotal() - bitmapItems->countFree());
}

template<typename T> uint64_t Table<T>::countTotalItems() {
    return (bitmapItems->countTotal());
}

#endif