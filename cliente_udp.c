#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define UDP_PORT 8081
#define BUFFER_SIZE 1024

int main() {

    int client_fd;

    struct sockaddr_in server_addr;

    char message[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    socklen_t server_len = sizeof(server_addr);

    // Criar socket UDP
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (client_fd == -1) {

        perror("Erro ao criar socket UDP");
        return 1;
    }

    printf("Socket UDP do cliente criado.\n");

    // Configurar servidor
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_PORT);

    if (inet_pton(
            AF_INET,
            "127.0.0.1",
            &server_addr.sin_addr
        ) <= 0) {

        perror("Endereço inválido");

        close(client_fd);

        return 1;
    }

    printf("Cliente UDP pronto.\n");

    while (1) {

        printf("\nDigite uma mensagem: ");
        fflush(stdout);

        if (fgets(
                message,
                BUFFER_SIZE,
                stdin
            ) == NULL) {

            break;
        }

        message[strcspn(
            message,
            "\n"
        )] = '\0';

        if (strcmp(message, "SAIR") == 0) {
            break;
        }

        if (strlen(message) == 0) {
            continue;
        }

        // Enviar mensagem
        if (sendto(
                client_fd,
                message,
                strlen(message),
                0,
                (struct sockaddr *)&server_addr,
                server_len
            ) == -1) {

            perror("Erro no sendto");
            break;
        }

        printf("Mensagem UDP enviada.\n");

        // Receber resposta
        memset(
            response,
            0,
            BUFFER_SIZE
        );

        int bytes_received = recvfrom(
            client_fd,
            response,
            BUFFER_SIZE - 1,
            0,
            NULL,
            NULL
        );

        if (bytes_received == -1) {

            perror("Erro no recvfrom");
            break;
        }

        response[bytes_received] = '\0';

        printf(
            "Resposta do servidor UDP: %s\n",
            response
        );
    }

    close(client_fd);

    printf("Cliente UDP encerrado.\n");

    return 0;
}