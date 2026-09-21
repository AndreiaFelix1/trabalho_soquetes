#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "replication.h"

#define BUFFER_SIZE 1024

int connect_to_replica(void) {

    int replica_fd;

    struct sockaddr_in replica_addr;

    replica_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (replica_fd == -1) {

        perror("Erro ao criar socket da réplica");
        return -1;
    }

    memset(
        &replica_addr,
        0,
        sizeof(replica_addr)
    );

    replica_addr.sin_family = AF_INET;
    replica_addr.sin_port = htons(REPLICA_PORT);

    if (inet_pton(
            AF_INET,
            "127.0.0.1",
            &replica_addr.sin_addr
        ) <= 0) {

        perror("Endereço da réplica inválido");

        close(replica_fd);

        return -1;
    }

    if (connect(
            replica_fd,
            (struct sockaddr *)&replica_addr,
            sizeof(replica_addr)
        ) == -1) {

        perror("Erro ao conectar à réplica");

        close(replica_fd);

        return -1;
    }

    printf("Conectado ao servidor réplica.\n");

    return replica_fd;
}


static int send_replication_command(
    int replica_fd,
    const char *command
) {

    char response[BUFFER_SIZE];

    if (send(
            replica_fd,
            command,
            strlen(command),
            0
        ) == -1) {

        perror("Erro ao enviar para réplica");

        return -1;
    }

    memset(
        response,
        0,
        BUFFER_SIZE
    );

    int bytes_received = recv(
        replica_fd,
        response,
        BUFFER_SIZE - 1,
        0
    );

    if (bytes_received <= 0) {

        perror("Erro ao receber resposta da réplica");

        return -1;
    }

    response[bytes_received] = '\0';

    printf(
        "Resposta da réplica: %s\n",
        response
    );

    return 0;
}


int replicate_create(
    int replica_fd,
    const char *id,
    const char *value
) {

    char command[BUFFER_SIZE];

    snprintf(
        command,
        BUFFER_SIZE,
        "CREATE %s %s",
        id,
        value
    );

    return send_replication_command(
        replica_fd,
        command
    );
}


int replicate_set(
    int replica_fd,
    const char *id,
    const char *value
) {

    char command[BUFFER_SIZE];

    snprintf(
        command,
        BUFFER_SIZE,
        "SET %s %s",
        id,
        value
    );

    return send_replication_command(
        replica_fd,
        command
    );
}


int replicate_reserve(
    int replica_fd,
    const char *id,
    int client_id
) {

    char command[BUFFER_SIZE];

    snprintf(
        command,
        BUFFER_SIZE,
        "RESERVE %s %d",
        id,
        client_id
    );

    return send_replication_command(
        replica_fd,
        command
    );
}


int replicate_release(
    int replica_fd,
    const char *id,
    int client_id
) {

    char command[BUFFER_SIZE];

    snprintf(
        command,
        BUFFER_SIZE,
        "RELEASE %s %d",
        id,
        client_id
    );

    return send_replication_command(
        replica_fd,
        command
    );
}


void close_replica(int replica_fd) {

    if (replica_fd >= 0) {

        close(replica_fd);

        printf("Conexão com a réplica encerrada.\n");
    }
}