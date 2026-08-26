#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "resource.h"

int save_resources(
    Resource resources[],
    int resource_count
);

int load_resources(
    Resource resources[],
    int *resource_count
);

#endif