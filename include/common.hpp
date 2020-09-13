#ifndef COMMON_HEADER
#define COMMON_HEADER

#include <stdint.h>
#include <time.h>

typedef int nrfs;
typedef char *nrfsFile;

#define MAX_MESSAGE_BLOCK_COUNT 10

typedef struct {
    uint16_t node_id;
    uint64_t offset;
    uint64_t size;
} file_pos_tuple;

struct file_pos_info {
    uint32_t len;
    file_pos_tuple tuple[MAX_MESSAGE_BLOCK_COUNT];
};

struct nrfsfileattr
{
    uint32_t mode; // 0-file, 1-directory
    uint64_t size;
    uint32_t time;
};

#define MAX_PATH_LENGTH 255
#define MAX_FILE_EXTENT_COUNT 20
#define BLOCK_SIZE (1 * 1024 * 1024)
#define MAX_FILE_NAME_LENGTH 50
#define MAX_DIRECTORY_COUNT 60

typedef uint64_t NodeHash;

typedef struct {
    NodeHash hashNode;
    uint32_t indexExtentStartBlock;
    uint32_t countExtentBlock;
} FileMetaTuple;

typedef struct {
    time_t timeLastModified;
    uint64_t count;
    FileMetaTuple tuple[MAX_FILE_EXTENT_COUNT];
} FileMeta;

typedef struct {
    char names[MAX_FILE_NAME_LENGTH];
    bool isDirectories;
} DirectoryMetaTuple;

typedef struct {
    uint64_t count;
    DirectoryMetaTuple tuple[MAX_DIRECTORY_COUNT];
}DirectoryMeta;

typedef DirectoryMeta nrfsfilelist;

static inline void NanosecondSleep(struct timespec *preTime, uint64_t diff) {
    struct timespec now;
    uint64_t temp;
    temp = 0;
    while (temp < diff) {
        clock_gettime(CLOCK_MONOTONIC, &now);
        temp = (now.tv_sec - preTime->tv_sec) * 1000000000 + now.tv_nsec - preTime->tv_nsec;
        temp = temp / 1000;
    }
}

#endif