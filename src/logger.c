#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "logger.h"

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_message(const char *message) {

    pthread_mutex_lock(&log_mutex);

    FILE *file = fopen("server.log", "a");

    if (file == NULL) {
        perror("Erro ao abrir arquivo de log");
        pthread_mutex_unlock(&log_mutex);
        return;
    }

    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);

    char timestamp[32];

    strftime(
        timestamp,
        sizeof(timestamp),
        "%Y-%m-%d %H:%M:%S",
        time_info
    );

    fprintf(
        file,
        "[%s] %s\n",
        timestamp,
        message
    );

    fclose(file);

    pthread_mutex_unlock(&log_mutex);
}