#include "bitmap.hpp"

Bitmap::Bitmap(uint64_t count, char *buffer) {
    if (count % 8 == 0) {
        if (buffer) {
            bytes = (uint8_t*)buffer;
            varCountTotal = count;
            varCountFree = 0;
            for (unsigned int index = 0; index < varCountTotal / 8; index ++) {
                for (int offset = 0; offset < 8; offset ++) {
                    if ((bytes[index] & (1 << (7 - offset))) == 0) {
                        varCountFree ++;
                    }
                }
            }
        } else {
            Debug::notifyError("Bitmap: buffer pointer is null\n");
            exit(EXIT_FAILURE);
        }
    } else {
        Debug::notifyError("Bitmap: buffer count should be times of eight.\n");
        exit(EXIT_FAILURE);
    }
}

Bitmap::~Bitmap() {
    ;
}

bool Bitmap::get(uint64_t pos, bool *status) {
    // check validity
    if (pos >= varCountTotal || status == NULL) {
        return false;
    }
    unsigned int index = pos / 8;
    int offset = pos % 8;
    *status = bytes[index] & (1 << (7 - offset));
    return true;
}

bool Bitmap::set(uint64_t pos) {
    if (pos >= varCountTotal) {
        return false;
    }
    unsigned int index = pos / 8;
    int offset = pos % 8;
    if ((bytes[index] & (1 << (7 - offset))) == 0) { // if the bit is zero
        bytes[index] |= (1 << (7 - offset));
        varCountFree --;
    }
    return true;
}

bool Bitmap::clear(uint64_t pos) {
    if (pos >= varCountTotal) {
        return false;
    }
    unsigned int index = pos / 8;
    int offset = pos % 8;
    if ((bytes[index] & (1 << (7 - offset))) != 0) {
        bytes[index] &= ~(1 << (7 - offset));
        varCountFree ++;
    }
    return true;
}

bool Bitmap::findFree(uint64_t *pos) {
    if (varCountFree == 0) {
        return false;
    }
    for (uint64_t index = 0; index < varCountTotal / 8; index ++) {
        for (int offset = 0; offset < 8; offset ++) {
            if ((bytes[index] & (1 << (7 - offset))) == 0) {
                *pos = index + offset;
                return true;
            }
        }
    }
    return false;
}

uint64_t Bitmap::countFree() {
    return varCountFree;
}

uint64_t Bitmap::countTotal() {
    return varCountTotal;
}