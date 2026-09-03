#include <stdio.h>

#include "replication.h"

int main() {

    int replica_fd;

    printf("Conectando à réplica...\n");

    replica_fd = connect_to_replica();

    if (replica_fd == -1) {

        printf("Não foi possível conectar à réplica.\n");

        return 1;
    }

    printf("Conexão estabelecida!\n");

    printf("\nEnviando CREATE...\n");

    if (replicate_create(
            replica_fd,
            "replica_test",
            "500"
        ) != 0) {

        printf("Erro ao replicar CREATE.\n");
    }

    printf("\nEnviando SET...\n");

    if (replicate_set(
            replica_fd,
            "replica_test",
            "999"
        ) != 0) {

        printf("Erro ao replicar SET.\n");
    }

    close_replica(replica_fd);

    return 0;
}