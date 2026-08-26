#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "resource.h"
#include "protocol.h"
#include <pthread.h>
#include "logger.h"
#include "persistence.h"
#include "monitor.h"
#include "auth.h"

#define PORT 8080
#define BUFFER_SIZE 1024

Resource resources[MAX_RESOURCES];
int resource_count = 0;

int next_client_id = 1;

pthread_mutex_t resources_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {

    int client_fd;
    int client_id;

} ClientArgs;

void *handle_client(void *arg) {

    ClientArgs *client_args = (ClientArgs *)arg;

    int client_fd = client_args->client_fd;
    int client_id = client_args->client_id;
    int authenticated = 0;
    char log_buffer[BUFFER_SIZE];

    free(client_args);

    printf(
        "Thread criada para o cliente ID: %d\n",
        client_id
    );

    while (1) {

        char buffer[BUFFER_SIZE];

        // Receber mensagem
        memset(buffer, 0, BUFFER_SIZE);

        int bytes_received = recv(
            client_fd,
            buffer,
            BUFFER_SIZE - 1,
            0
        );

        if (bytes_received == -1) {

            perror("Erro no recv");
            break;
        }

        if (bytes_received == 0) {

            printf(
                "Cliente ID %d encerrou a conexão.\n",
                client_id
            );

            break;
        }

        buffer[bytes_received] = '\0';

        printf(
            "Cliente ID %d: %s\n",
            client_id,
            buffer
        );

        snprintf(
            log_buffer,
            BUFFER_SIZE,
            "Cliente ID %d -> %s",
            client_id,
            buffer
        );

        log_message(log_buffer);

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

        if (items == 3 && strcmp(command, "LOGIN") == 0) {

            int result = authenticate_user(id, value);

            if (result == 0) {

                authenticated = 1;

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "OK AUTHENTICATED"
                );

            } else {

                authenticated = 0;

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR INVALID_CREDENTIALS"
                );
            }

        if (!(items == 1 && strcmp(command, "STATUS") == 0)) {
            monitor_command_received();
        }

        //char response[BUFFER_SIZE];

        if (!authenticated) {

            snprintf(
                response,
                BUFFER_SIZE,
                "ERROR AUTH_REQUIRED"
            );

        } else if (items == 3 && strcmp(command, CMD_CREATE) == 0) {

            pthread_mutex_lock(&resources_mutex);

            int result = create_resource(
                resources,
                &resource_count,
                id,
                value
            );

            pthread_mutex_unlock(&resources_mutex);

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

        } else if (items == 2 && strcmp(command, CMD_GET) == 0) {

            pthread_mutex_lock(&resources_mutex);

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

            pthread_mutex_unlock(&resources_mutex);

        } else if (items == 3 && strcmp(command, CMD_SET) == 0) {


            pthread_mutex_lock(&resources_mutex);

            int result = set_resource(
                resources,
                resource_count,
                id,
                value
            );


            pthread_mutex_unlock(&resources_mutex);

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

            } else {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "ERROR RESOURCE_NOT_FOUND"
                );
            }

        } else if (items == 1 && strcmp(command, CMD_LIST) == 0) {

            pthread_mutex_lock(&resources_mutex);

            list_resources(
                resources,
                resource_count,
                response,
                BUFFER_SIZE
            );

            pthread_mutex_unlock(&resources_mutex);

        } else if (items == 2 && strcmp(command, CMD_RESERVE) == 0) {

            pthread_mutex_lock(&resources_mutex);

            int result = reserve_resource(
                resources,
                resource_count,
                id,
                client_id
            );

            pthread_mutex_unlock(&resources_mutex);

            if (result == 0) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "OK RESOURCE_RESERVED"
                );

                save_resources(
                    resources,
                    resource_count
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

            pthread_mutex_lock(&resources_mutex);

            int result = release_resource(
                resources,
                resource_count,
                id,
                client_id
            );

            pthread_mutex_unlock(&resources_mutex);

            if (result == 0) {

                snprintf(
                    response,
                    BUFFER_SIZE,
                    "OK RESOURCE_RELEASED"
                );

                save_resources(
                    resources,
                    resource_count
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
        
        }

        } else if (items == 1 && strcmp(command, "STATUS") == 0) {


            monitor_get_status(
                response,
                BUFFER_SIZE
            );

        } else {

            snprintf(
                response,
                BUFFER_SIZE,
                "ERROR INVALID_COMMAND"
            );
        }

        snprintf(
            log_buffer,
            BUFFER_SIZE,
            "Cliente ID %d <- %s",
            client_id,
            response
        );

        log_message(log_buffer);

        if (strncmp(response, "OK", 2) == 0) {

            monitor_command_success();

        } else {

            monitor_command_error();
        }

        // Enviar resposta
        if (send(
                client_fd,
                response,
                strlen(response),
                0
            ) == -1) {

            perror("Erro no send");
            break;
        }
    }

    pthread_mutex_lock(&resources_mutex);

    release_resources_by_client(
        resources,
        resource_count,
        client_id
    );

    save_resources(
        resources,
        resource_count
    );

    snprintf(
        log_buffer,
        BUFFER_SIZE,
        "Recursos do cliente ID %d liberados automaticamente",
        client_id
    );

    log_message(log_buffer);

    pthread_mutex_unlock(&resources_mutex);

    monitor_client_disconnected();

    close(client_fd);

    printf(
        "Cliente ID %d desconectado.\n",
        client_id
    );

    snprintf(
        log_buffer,
        BUFFER_SIZE,
        "Cliente ID %d desconectado",
        client_id
    );

    log_message(log_buffer);

    return NULL;
}

int main() {

    monitor_init();

    int server_fd;
    int client_fd;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    socklen_t client_len = sizeof(client_addr);

    if (load_resources(resources, &resource_count) != 0) {

        fprintf(
            stderr,
            "Erro ao carregar os recursos.\n"
        );

        return 1;
    }

    printf(
        "Recursos carregados: %d\n",
        resource_count
    );

    // Criar o socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("Erro ao criar socket");
        return 1;
    }

    printf("Socket criado com sucesso.\n");

    // Configurar o endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Associar o socket à porta
    if (bind(server_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {

        perror("Erro no bind");
        close(server_fd);
        return 1;
    }

    printf("Servidor associado à porta %d.\n", PORT);

    // Colocar o servidor para escutar
    if (listen(server_fd, 5) == -1) {

        perror("Erro no listen");
        close(server_fd);
        return 1;
    }

    printf("Servidor aguardando conexões...\n");

    // Aceitar um cliente
    while (1) {

        // Aceitar um cliente
        client_len = sizeof(client_addr);

        client_fd = accept(
            server_fd,
            (struct sockaddr *)&client_addr,
            &client_len
        );

        if (client_fd == -1) {

            perror("Erro no accept");
            continue;
        }

        int client_id = next_client_id++;

        printf(
            "Cliente conectado! ID: %d\n",
            client_id
        );

        monitor_client_connected();

        char log_buffer[BUFFER_SIZE];

        snprintf(
            log_buffer,
            BUFFER_SIZE,
            "Cliente ID %d conectado",
            client_id
        );

        log_message(log_buffer);

        ClientArgs *client_args = malloc(sizeof(ClientArgs));

        if (client_args == NULL) {

            perror("Erro ao alocar memória");

            close(client_fd);

            continue;
        }

        client_args->client_fd = client_fd;
        client_args->client_id = client_id;

        pthread_t thread;

        if (pthread_create(
                &thread,
                NULL,
                handle_client,
                client_args
            ) != 0) {

            perror("Erro ao criar thread");

            free(client_args);
            close(client_fd);

            continue;
        }

        pthread_detach(thread);

    }

    // Fechar o socket do servidor
    close(server_fd);

    return 0;
}