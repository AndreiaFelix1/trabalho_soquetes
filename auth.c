#include <string.h>
#include "auth.h"

int authenticate_user(
    const char *username,
    const char *password
) {

    if (strcmp(username, "andreia") == 0 &&
        strcmp(password, "1234") == 0) {

        return 0;
    }

    return -1;
}