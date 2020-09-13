#ifndef GLOBAL_HEADER
#define GLOBAL_HEADER

#include <stdint.h>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <sys/syscall.h>
#include <unistd.h>
#include "common.hpp"

using namespace std;

static inline uint32_t gettid() {
    return (uint32_t)syscall(SYS_gettid);
}

#define CLIENT_MESSAGE_SIZE 4096
#define MAX_CLIENT_NUMBER 1024
#define SERVER_MESSAGE_SIZE CLIENT_MESSAGE_SIZE
#define SERVER_MESSAGE_NUM 8
#define METADATA_SIZE (1024 * 1024 * 1024)
#define LOCALLOGSIZE (40 * 1024 * 1024)
#define DISTRIBUTEDLOGSIZE (1024 * 1024)

#define TRANSACTION_CD 1

typedef enum {
    MESSAGE_ADDMETATODIRECTORY,
    MESSAGE_REMOVEMETAFROMDIRECTORY,
    MESSAGE_MKNODWITHMETA
} Message;


#endif