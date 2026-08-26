#include <stdio.h>
#include "auth.h"

int main() {

    if (authenticate_user("andreia", "1234") == 0) {

        printf("Login correto.\n");

    } else {

        printf("Login incorreto.\n");
    }

    if (authenticate_user("andreia", "9999") == 0) {

        printf("Login correto.\n");

    } else {

        printf("Login incorreto.\n");
    }

    return 0;
}