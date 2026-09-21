#include <stdio.h>
#include <string.h>
#include "resource.h"
#include "persistence.h"

int main() {

    Resource resources[MAX_RESOURCES];
    int resource_count = 2;

    // Criar recursos de teste
    strcpy(resources[0].id, "sala1");
    strcpy(resources[0].value, "100");
    resources[0].reserved = 0;
    resources[0].client_id = -1;

    strcpy(resources[1].id, "sala2");
    strcpy(resources[1].value, "200");
    resources[1].reserved = 0;
    resources[1].client_id = -1;

    // Salvar
    printf("Salvando recursos...\n");

    if (save_resources(resources, resource_count) != 0) {

        printf("Erro ao salvar recursos.\n");
        return 1;
    }

    printf("Recursos salvos com sucesso.\n");

    // Limpar memória
    resource_count = 0;

    // Carregar
    printf("Carregando recursos...\n");

    if (load_resources(resources, &resource_count) != 0) {

        printf("Erro ao carregar recursos.\n");
        return 1;
    }

    printf(
        "Recursos carregados: %d\n",
        resource_count
    );

    for (int i = 0; i < resource_count; i++) {

        printf(
            "ID: %s | Valor: %s | Reservado: %d | Cliente: %d\n",
            resources[i].id,
            resources[i].value,
            resources[i].reserved,
            resources[i].client_id
        );
    }

    return 0;
}