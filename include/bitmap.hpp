#ifndef BITMAP_HEADER
#define BITMAP_HEADER

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "debug.hpp"

class Bitmap {
private:
    uint8_t *bytes;
    uint64_t varCountFree;
    uint64_t varCountTotal;

public:
    bool get(uint64_t pos, bool *status);
    bool set(uint64_t pos);
    bool clear(uint64_t pos);
    bool findFree(uint64_t *pos);
    uint64_t countFree();
    uint64_t countTotal();
    Bitmap(uint64_t count, char *buffer);
    ~Bitmap();
};

#endif