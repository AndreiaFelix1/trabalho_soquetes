#ifndef RESOURCE_H
#define RESOURCE_H

#define MAX_RESOURCES 100
#define MAX_ID_SIZE 50
#define MAX_VALUE_SIZE 256

typedef struct {

    char id[MAX_ID_SIZE];
    char value[MAX_VALUE_SIZE];

    int reserved;
    int client_id;

} Resource;

int create_resource(

    Resource resources[],
    int *resource_count,
    const char *id,
    const char *value
);

Resource *get_resource(
    Resource resources[],
    int resource_count,
    const char *id
);

int set_resource(
    Resource resources[],
    int resource_count,
    const char *id,
    const char *value
);

void list_resources(
    Resource resources[],
    int resource_count,
    char *response,
    int response_size
);

int reserve_resource(
    Resource resources[],
    int resource_count,
    const char *id,
    int client_id
);

int release_resource(
    Resource resources[],
    int resource_count,
    const char *id,
    int client_id
);

void release_resources_by_client(
    Resource resources[],
    int resource_count,
    int client_id
);

#endif