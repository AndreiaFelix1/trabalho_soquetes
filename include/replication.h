#ifndef REPLICATION_H
#define REPLICATION_H

#include "resource.h"

#define REPLICA_PORT 8082

int connect_to_replica(void);

int replicate_create(
    int replica_fd,
    const char *id,
    const char *value
);

int replicate_set(
    int replica_fd,
    const char *id,
    const char *value
);

int replicate_reserve(
    int replica_fd,
    const char *id,
    int client_id
);

int replicate_release(
    int replica_fd,
    const char *id,
    int client_id
);

void close_replica(int replica_fd);

#endif