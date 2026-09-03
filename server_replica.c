#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "resource.h"
#include "protocol.h"
#include "persistence.h"

#define REPLICA_PORT 8082
#define BUFFER_SIZE 1024

#define REPLICA_DATA_FILE "replica_resources.dat"

void save_replica_resources(
    Resource resources[],
    int resource_count
) {

    FILE *file = fopen(
        REPLICA_DATA_FILE,
        "w"
    );

    if (file == NULL) {

        perror("Erro ao abrir arquivo da réplica");
        return;
    }

    for (int i = 0; i < resource_count; i++) {

        fprintf(
            file,
            "%s %s\n",
            resources[i].id,
            resources[i].value
        );
    }

    fclose(file);
}

int load_replica_resources(
    Resource resources[],
    int *resource_count
) {

    FILE *file = fopen(
        REPLICA_DATA_FILE,
        "r"
    );

    if (file == NULL) {

        *resource_count = 0;
        return -1;
    }

    *resource_count = 0;

    while (
        *resource_count < MAX_RESOURCES &&
        fscanf(
            file,
            "%49s %255s",
            resources[*resource_count].id,
            resources[*resource_count].value
        ) == 2
    ) {

        (*resource_count)++;
    }

    fclose(file);

    return 0;
}

int main() {

    int server_fd;
    int client_fd;

    Resource resources[MAX_RESOURCES];
    int resource_count = 0;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    socklen_t client_len = sizeof(client_addr);

    char buffer[BUFFER_SIZE];

    // Carregar recursos persistidos
    if (load_replica_resources(
            resources,
            &resource_count
        ) != 0) {

        printf("Nenhum recurso carregado.\n");

    } else {

        printf(
            "Recursos carregados: %d\n",
            resource_count
        );
    }

    // Criar socket TCP
    server_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (server_fd == -1) {

        perror("Erro ao criar socket da réplica");

        return 1;
    }

    printf(
        "Socket da réplica criado.\n"
    );

    memset(
        &server_addr,
        0,
        sizeof(server_addr)
    );

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(REPLICA_PORT);

    if (bind(
            server_fd,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)
        ) == -1) {

        perror("Erro no bind da réplica");

        close(server_fd);

        return 1;
    }

    printf(
        "Réplica associada à porta %d.\n",
        REPLICA_PORT
    );

    if (listen(server_fd, 1) == -1) {

        perror("Erro no listen da réplica");

        close(server_fd);

        return 1;
    }

    printf(
        "Réplica aguardando conexão do servidor principal...\n"
    );

    client_len = sizeof(client_addr);

    client_fd = accept(
        server_fd,
        (struct sockaddr *)&client_addr,
        &client_len
    );

    if (client_fd == -1) {

        perror("Erro no accept da réplica");

        close(server_fd);

        return 1;
    }

    printf(
        "Servidor principal conectado à réplica!\n"
    );

    while (1) {

        memset(
            buffer,
            0,
            BUFFER_SIZE
        );

        int bytes_received = recv(
            client_fd,
            buffer,
            BUFFER_SIZE - 1,
            0
        );

        if (bytes_received == -1) {

            perror("Erro no recv da réplica");

            break;
        }

        if (bytes_received == 0) {

            printf(
                "Servidor principal desconectado.\n"
            );

            break;
        }

        buffer[bytes_received] = '\0';

        printf(
            "Comando recebido: %s\n",
            buffer
        );

        char command[50];
        char id[MAX_ID_SIZE];
        char value[MAX_VALUE_SIZE];

        int items = sscanf(
            buffer,
            "%49s %49s %255s",
            command,
            id,
            value
        );

        char response[BUFFER_SIZE];

        if (
            items == 3 &&
            strcmp(command, CMD_CREATE) == 0
        ) {

            int result = create_resource(
                resources,
                &resource_count,
                id,
                value
            );

            if (result == 0) {

                save_replica_resources(
                    resources,
                    resource_count
                );

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "OK REPLICATED"
                );

            } else {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR REPLICATION_FAILED"
                );
            }

        } else if (
            items == 3 &&
            strcmp(command, CMD_SET) == 0
        ) {

            int result = set_resource(
                resources,
                resource_count,
                id,
                value
            );

            if (result == 0) {

                save_replica_resources(
                    resources,
                    resource_count
                );

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "OK REPLICATED"
                );

            } else {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR RESOURCE_NOT_FOUND"
                );
            }

        } else {

            snprintf(
                response,
                BUFFER_SIZE,
                "ERROR INVALID_REPLICATION_COMMAND"
            );
        }

        if (send(
                client_fd,
                response,
                strlen(response),
                0
            ) == -1) {

            perror("Erro no send da réplica");

            break;
        }
    }

    close(client_fd);
    close(server_fd);

    return 0;
}