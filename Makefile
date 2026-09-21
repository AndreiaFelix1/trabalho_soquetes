CC = gcc

CFLAGS = -Wall -Wextra -Iinclude -pthread

COMMON_SRC = \
	src/resource.c \
	src/protocol.c \
	src/logger.c \
	src/persistence.c \
	src/monitor.c \
	src/auth.c \
	src/replication.c

SERVER_TCP = server_tcp
SERVER_UDP = server_udp
SERVER_REPLICA = server_replica
CLIENT = cliente
CLIENT_UDP = cliente_udp

all: $(SERVER_TCP) $(SERVER_UDP) $(SERVER_REPLICA) $(CLIENT) $(CLIENT_UDP)

$(SERVER_TCP):
	$(CC) $(CFLAGS) -o $@ src/server_tcp.c $(COMMON_SRC)

$(SERVER_UDP):
	$(CC) $(CFLAGS) -o $@ src/server_udp.c $(COMMON_SRC)

$(SERVER_REPLICA):
	$(CC) $(CFLAGS) -o $@ src/server_replica.c $(COMMON_SRC)

$(CLIENT):
	$(CC) $(CFLAGS) -o $@ src/cliente.c $(COMMON_SRC)

$(CLIENT_UDP):
	$(CC) $(CFLAGS) -o $@ src/cliente_udp.c $(COMMON_SRC)

clean:
	rm -f $(SERVER_TCP) $(SERVER_UDP) $(SERVER_REPLICA) $(CLIENT) $(CLIENT_UDP)
	rm -f src/*.o
	rm -f src/test_auth src/test_persistence src/test_replication

run-tcp:
	./$(SERVER_TCP)

run-udp:
	./$(SERVER_UDP)

run-replica:
	./$(SERVER_REPLICA)

run-client:
	./$(CLIENT)

run-client-udp:
	./$(CLIENT_UDP)
