#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "monitor.h"

static int clients_connected = 0;
static int total_clients = 0;

static int commands_received = 0;
static int commands_success = 0;
static int commands_error = 0;

static time_t start_time;

static pthread_mutex_t monitor_mutex =
    PTHREAD_MUTEX_INITIALIZER;


void monitor_init() {

    pthread_mutex_lock(&monitor_mutex);

    clients_connected = 0;
    total_clients = 0;

    commands_received = 0;
    commands_success = 0;
    commands_error = 0;

    start_time = time(NULL);

    pthread_mutex_unlock(&monitor_mutex);
}


void monitor_client_connected() {

    pthread_mutex_lock(&monitor_mutex);

    clients_connected++;
    total_clients++;

    pthread_mutex_unlock(&monitor_mutex);
}


void monitor_client_disconnected() {

    pthread_mutex_lock(&monitor_mutex);

    if (clients_connected > 0) {
        clients_connected--;
    }

    pthread_mutex_unlock(&monitor_mutex);
}


void monitor_command_received() {

    pthread_mutex_lock(&monitor_mutex);

    commands_received++;

    pthread_mutex_unlock(&monitor_mutex);
}


void monitor_command_success() {

    pthread_mutex_lock(&monitor_mutex);

    commands_success++;

    pthread_mutex_unlock(&monitor_mutex);
}


void monitor_command_error() {

    pthread_mutex_lock(&monitor_mutex);

    commands_error++;

    pthread_mutex_unlock(&monitor_mutex);
}

void monitor_get_status(
    char response[],
    int buffer_size
) {

    pthread_mutex_lock(&monitor_mutex);

    time_t current_time = time(NULL);

    long uptime = (long)difftime(
        current_time,
        start_time
    );

    snprintf(
        response,
        buffer_size,
        "OK STATUS "
        "CLIENTS_CONNECTED %d "
        "TOTAL_CLIENTS %d "
        "COMMANDS_RECEIVED %d "
        "COMMANDS_SUCCESS %d "
        "COMMANDS_ERROR %d "
        "UPTIME %ld",
        clients_connected,
        total_clients,
        commands_received,
        commands_success,
        commands_error,
        uptime
    );

    pthread_mutex_unlock(&monitor_mutex);
}