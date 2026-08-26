#include <stdio.h>
#include "persistence.h"

#define DATA_FILE "resources.dat"

int save_resources(
    Resource resources[],
    int resource_count
) {

    FILE *file = fopen(DATA_FILE, "w");

    if (file == NULL) {
        perror("Erro ao abrir arquivo para salvar recursos");
        return -1;
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

    return 0;
}

int load_resources(
    Resource resources[],
    int *resource_count
) {

    FILE *file = fopen(DATA_FILE, "r");

    if (file == NULL) {

        // Arquivo ainda não existe.
        // Isso não é considerado um erro.
        *resource_count = 0;

        return 0;
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

        resources[*resource_count].reserved = 0;
        resources[*resource_count].client_id = -1;

        (*resource_count)++;
    }

    fclose(file);

    return 0;
}