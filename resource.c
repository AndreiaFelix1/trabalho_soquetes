#include <stdio.h>
#include <string.h>

#include "resource.h"

int create_resource(
    Resource resources[],
    int *resource_count,
    const char *id,
    const char *value
) {

    // Verificar se já atingimos o limite
    if (*resource_count >= MAX_RESOURCES) {
        return -1;
    }

    // Verificar se o recurso já existe
    for (int i = 0; i < *resource_count; i++) {

        if (strcmp(resources[i].id, id) == 0) {
            return -2;
        }
    }

    // Criar o novo recurso
    snprintf(
        resources[*resource_count].id,
        MAX_ID_SIZE,
        "%s",
        id
    );

    snprintf(
        resources[*resource_count].value,
        MAX_VALUE_SIZE,
        "%s",
        value
    );

    // Inicialmente o recurso está livre
    resources[*resource_count].reserved = 0;

    // Nenhum cliente possui a reserva
    resources[*resource_count].client_id = -1;

    // Incrementar quantidade de recursos
    (*resource_count)++;

    return 0;
}

Resource *get_resource(
    Resource resources[],
    int resource_count,
    const char *id
) {

    for (int i = 0; i < resource_count; i++) {

        if (strcmp(resources[i].id, id) == 0) {
            return &resources[i];
        }
    }

    return NULL;
}

int set_resource(
    Resource resources[],
    int resource_count,
    const char *id,
    const char *value
) {

    Resource *resource = get_resource(
        resources,
        resource_count,
        id
    );

    if (resource == NULL) {
        return -1;
    }

    snprintf(
        resource->value,
        MAX_VALUE_SIZE,
        "%s",
        value
    );

    return 0;
}

void list_resources(
    Resource resources[],
    int resource_count,
    char *response,
    int response_size
) {

    snprintf(
        response,
        response_size,
        "OK RESOURCES"
    );

    for (int i = 0; i < resource_count; i++) {

        char resource_info[MAX_ID_SIZE + MAX_VALUE_SIZE + 3];

        snprintf(
            resource_info,
            sizeof(resource_info),
            " %s=%s",
            resources[i].id,
            resources[i].value
        );

        strncat(
            response,
            resource_info,
            response_size - strlen(response) - 1
        );
    }
}

int reserve_resource(
    Resource resources[],
    int resource_count,
    const char *id,
    int client_id
) {

    Resource *resource = get_resource(
        resources,
        resource_count,
        id
    );

    if (resource == NULL) {
        return -1;
    }

    if (resource->reserved == 1) {
        return -2;
    }

    resource->reserved = 1;
    resource->client_id = client_id;

    return 0;
}

int release_resource(
    Resource resources[],
    int resource_count,
    const char *id,
    int client_id
) {

    Resource *resource = get_resource(
        resources,
        resource_count,
        id
    );

    // Recurso não existe
    if (resource == NULL) {
        return -1;
    }

    // Recurso existe, mas não está reservado
    if (resource->reserved == 0) {
        return -2;
    }

    // Recurso está reservado por outro cliente
    if (resource->client_id != client_id) {
        return -3;
    }

    // Libera a reserva
    resource->reserved = 0;
    resource->client_id = -1;

    return 0;
}

void release_resources_by_client(
    Resource resources[],
    int resource_count,
    int client_id
) {

    for (int i = 0; i < resource_count; i++) {

        if (
            resources[i].reserved == 1 &&
            resources[i].client_id == client_id
        ) {

            resources[i].reserved = 0;
            resources[i].client_id = -1;

            printf(
                "Recurso %s liberado automaticamente do cliente %d.\n",
                resources[i].id,
                client_id
            );
        }
    }
}