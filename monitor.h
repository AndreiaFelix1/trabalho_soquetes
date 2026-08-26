#ifndef MONITOR_H
#define MONITOR_H

void monitor_init();

void monitor_client_connected();
void monitor_client_disconnected();

void monitor_command_received();
void monitor_command_success();
void monitor_command_error();

void monitor_get_status(
    char response[],
    int buffer_size
);

#endif