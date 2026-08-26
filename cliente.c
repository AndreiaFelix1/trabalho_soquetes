#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {

    int client_fd;

    struct sockaddr_in server_addr;

    // Criar o socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd == -1) {
        perror("Erro ao criar socket");
        return 1;
    }

    printf("Socket do cliente criado.\n");

    // Configurar endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Endereço do servidor
    if (inet_pton(
            AF_INET,
            "127.0.0.1",
            &server_addr.sin_addr
        ) <= 0) {

        perror("Endereço inválido");
        close(client_fd);
        return 1;
    }

    // Conectar ao servidor
    if (connect(
            client_fd,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)
        ) == -1) {

        perror("Erro ao conectar");
        close(client_fd);
        return 1;
    }

    printf("Conectado ao servidor!\n");

    // Buffers para comunicação
    char message[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // Manter a conexão aberta
    while (1) {

        printf("\nDigite um comando: ");
        fflush(stdout);

        // Ler comando digitado
        if (fgets(message, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // Remover o \n
        message[strcspn(message, "\n")] = '\0';

        // Encerrar cliente
        if (strcmp(message, "SAIR") == 0) {
            break;
        }

        // Ignorar entrada vazia
        if (strlen(message) == 0) {
            continue;
        }

        // Enviar comando
        if (send(
                client_fd,
                message,
                strlen(message),
                0
            ) == -1) {

            perror("Erro no send");
            break;
        }

        printf("Mensagem enviada.\n");

        // Limpar buffer da resposta
        memset(response, 0, BUFFER_SIZE);

        // Receber resposta
        int bytes_received = recv(
            client_fd,
            response,
            BUFFER_SIZE - 1,
            0
        );

        if (bytes_received == -1) {

            perror("Erro no recv");
            break;
        }

        if (bytes_received == 0) {

            printf("Servidor encerrou a conexão.\n");
            break;
        }

        // Finalizar string
        response[bytes_received] = '\0';

        printf(
            "Resposta do servidor: %s\n",
            response
        );
    }

    // Fechar conexão
    close(client_fd);

    printf("Cliente encerrado.\n");

    return 0;
}