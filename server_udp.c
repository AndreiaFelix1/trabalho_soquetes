#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "resource.h"
#include "protocol.h"
#include "auth.h"
#include "persistence.h"
#include "monitor.h"
#include "replication.h"
#define UDP_PORT 8081
#define BUFFER_SIZE 1024

#define MAX_UDP_CLIENTS 100

typedef struct {
    struct sockaddr_in address;
    int authenticated;
} UDPClient;

int main() {

    int server_fd;

    Resource resources[MAX_RESOURCES];
    int resource_count = 0;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    socklen_t client_len = sizeof(client_addr);

    char buffer[BUFFER_SIZE];

    UDPClient udp_clients[MAX_UDP_CLIENTS];
    int udp_client_count = 0;


    memset(
        udp_clients,
        0,
        sizeof(udp_clients)
    );

    if (load_resources(resources, &resource_count) != 0) {

        printf("Nenhum recurso carregado.\n");

    } else {

        printf(
            "Recursos carregados: %d\n",
            resource_count
        );
    }

    monitor_init();

    // Criar socket UDP
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);



    if (server_fd == -1) {

        perror("Erro ao criar socket UDP");
        return 1;
    }

    printf("Socket UDP criado com sucesso.\n");

    // Configurar endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(UDP_PORT);

    // Associar socket à porta
    if (bind(
            server_fd,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)
        ) == -1) {

        perror("Erro no bind UDP");

        close(server_fd);

        return 1;
    }

    printf(
        "Servidor UDP associado à porta %d.\n",
        UDP_PORT
    );

    printf("Servidor UDP aguardando mensagens...\n");

    while (1) {

        memset(buffer, 0, BUFFER_SIZE);

        client_len = sizeof(client_addr);

        // Receber mensagem
        int bytes_received = recvfrom(
            server_fd,
            buffer,
            BUFFER_SIZE - 1,
            0,
            (struct sockaddr *)&client_addr,
            &client_len
        );

        if (bytes_received == -1) {

            perror("Erro no recvfrom");
            continue;
        }

        buffer[bytes_received] = '\0';

        monitor_command_received();

        printf(
            "Mensagem UDP recebida: %s\n",
            buffer
        );

        int client_index = -1;

        for (int i = 0; i < udp_client_count; i++) {

            if (
                udp_clients[i].address.sin_addr.s_addr ==
                    client_addr.sin_addr.s_addr &&
                udp_clients[i].address.sin_port ==
                    client_addr.sin_port
            ) {

                client_index = i;
                break;
            }
        }

        char response[BUFFER_SIZE];

        if (client_index == -1) {

            if (udp_client_count >= MAX_UDP_CLIENTS) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR UDP_CLIENT_LIMIT"
                );

            } else {

                client_index = udp_client_count++;

                udp_clients[client_index].address = client_addr;
                udp_clients[client_index].authenticated = 0;
            }
        }

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

        if (items == 3 && strcmp(command, "LOGIN") == 0) {

            int result = authenticate_user(id, value);

            if (result == 0) {

                udp_clients[client_index].authenticated = 1;

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "OK AUTHENTICATED"
                );

            } else {

                udp_clients[client_index].authenticated = 0;

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR INVALID_CREDENTIALS"
                );
            }

        } else if (items == 1 && strcmp(command, "STATUS") == 0) {

            monitor_get_status(
                response,
                BUFFER_SIZE
            );

        } else if (!udp_clients[client_index].authenticated) {

            snprintf(
                response,
                BUFFER_SIZE,
                "ERROR AUTH_REQUIRED"
            );

        } else if (items == 2 && strcmp(command, CMD_GET) == 0) {

            Resource *resource = get_resource(
                resources,
                resource_count,
                id
            );

            if (resource == NULL) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR RESOURCE_NOT_FOUND"
                );

            } else {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "OK VALUE %s",
                    resource->value
                );
            }

        } else if (items == 1 && strcmp(command, CMD_LIST) == 0) {

            list_resources(
                resources,
                resource_count,
                response,
                BUFFER_SIZE
            );

        } else if (items == 3 && strcmp(command, CMD_CREATE) == 0) {

            int result = create_resource(
                resources,
                &resource_count,
                id,
                value
            );

            if (result == 0) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "OK RESOURCE_CREATED"
                );

                save_resources(
                    resources,
                    resource_count
                );

            } else if (result == -2) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR RESOURCE_ALREADY_EXISTS"
                );

            } else {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR RESOURCE_LIMIT_REACHED"
                );
            }

        } else if (items == 3 && strcmp(command, CMD_SET) == 0) {

            int result = set_resource(
                resources,
                resource_count,
                id,
                value
            );

            if (result == 0) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "OK VALUE_UPDATED"
                );

                save_resources(
                    resources,
                    resource_count
                );

                if (replica_fd != -1) {

                    replicate_set(
        replica_fd,
        id,
        value
    );
}

            } else {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR RESOURCE_NOT_FOUND"
                );
            }

        } else if (items == 2 && strcmp(command, CMD_RESERVE) == 0) {

            int result = reserve_resource(
                resources,
                resource_count,
                id,
                client_index + 1
            );

            if (result == 0) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "OK RESOURCE_RESERVED"
                );

            } else if (result == -1) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR RESOURCE_NOT_FOUND"
                );

            } else {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR RESOURCE_ALREADY_RESERVED"
                );
            }

        } else if (items == 2 && strcmp(command, CMD_RELEASE) == 0) {

            int result = release_resource(
                resources,
                resource_count,
                id,
                client_index + 1
            );

            if (result == 0) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "OK RESOURCE_RELEASED"
                );

            } else if (result == -1) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR RESOURCE_NOT_FOUND"
                );

            } else if (result == -2) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR RESOURCE_NOT_RESERVED"
                );

            } else {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR NOT_OWNER"
                );
            }

        } else {

            snprintf(
                response,
                BUFFER_SIZE,
                "ERROR INVALID_COMMAND"
            );
        }

        if (strncmp(response, "OK", 2) == 0) {

            monitor_command_success();

        } else {

            monitor_command_error();
        }

        // Enviar resposta
        if (sendto(
                server_fd,
                response,
                strlen(response),
                0,
                (struct sockaddr *)&client_addr,
                client_len
            ) == -1) {

            perror("Erro no sendto");
            continue;
        }

        printf(
            "Resposta UDP enviada: %s\n",
            response
        );
    }

    close(server_fd);

    return 0;
}