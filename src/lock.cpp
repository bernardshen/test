#include "lock.h"
#include "RPCServer.hpp"

extern RPCServer *server;
LockService::LockService(uint64_t _MetaDataBaseAddress)
: MetaDataBaseAddress(_MetaDataBaseAddress) {}

uint64_t LockService::WriteLock(uint16_t NodeId, uint64_t Address) {
    
}

