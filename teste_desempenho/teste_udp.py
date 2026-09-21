import socket
import threading
import time
import os
from datetime import datetime

HOST = "127.0.0.1"
PORT = 8081

USUARIO = "andreia"
SENHA = "1234"

CLIENTES = [10, 50, 100]

LOG_FILE = "logs/desempenho_udp.log"

log_lock = threading.Lock()


def timestamp():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]


def log(message):
    with log_lock:
        with open(LOG_FILE, "a", encoding="utf-8") as f:
            f.write(f"[{timestamp()}] {message}\n")


def enviar_comando(sock, endereco, client_id, comando):
    inicio = time.perf_counter()

    log(f"Cliente ID {client_id} -> {comando}")

    sock.sendto(
        comando.encode(),
        endereco
    )

    resposta, _ = sock.recvfrom(4096)

    resposta = resposta.decode().strip()

    fim = time.perf_counter()

    tempo_ms = (fim - inicio) * 1000

    log(f"Cliente ID {client_id} <- {resposta}")

    return resposta, tempo_ms


def cliente_udp(client_id, resultados):
    try:

        sock = socket.socket(
            socket.AF_INET,
            socket.SOCK_DGRAM
        )

        # Porta local diferente para cada cliente
        sock.bind(("127.0.0.1", 0))

        endereco = (HOST, PORT)

        log(f"Cliente ID {client_id} conectado")

        # LOGIN
        resposta, _ = enviar_comando(
            sock,
            endereco,
            client_id,
            f"LOGIN {USUARIO} {SENHA}"
        )

        if not resposta.startswith("OK"):
            log(
                f"Cliente ID {client_id} encerrado - "
                f"falha na autenticacao"
            )
            sock.close()
            return

        recurso = f"sala_udp_{client_id}"

        tempos = []

        # CREATE
        resposta, tempo = enviar_comando(
            sock,
            endereco,
            client_id,
            f"CREATE {recurso} 100"
        )
        tempos.append(("CREATE", tempo))

        # GET
        resposta, tempo = enviar_comando(
            sock,
            endereco,
            client_id,
            f"GET {recurso}"
        )
        tempos.append(("GET", tempo))

        # SET
        resposta, tempo = enviar_comando(
            sock,
            endereco,
            client_id,
            f"SET {recurso} 200"
        )
        tempos.append(("SET", tempo))

        # LIST
        resposta, tempo = enviar_comando(
            sock,
            endereco,
            client_id,
            "LIST"
        )
        tempos.append(("LIST", tempo))

        # RESERVE
        resposta, tempo = enviar_comando(
            sock,
            endereco,
            client_id,
            f"RESERVE {recurso}"
        )
        tempos.append(("RESERVE", tempo))

        # RELEASE
        resposta, tempo = enviar_comando(
            sock,
            endereco,
            client_id,
            f"RELEASE {recurso}"
        )
        tempos.append(("RELEASE", tempo))

        for comando, tempo in tempos:
            resultados.append((comando, tempo))

        log(f"Cliente ID {client_id} desconectado")

        sock.close()

    except Exception as e:

        log(
            f"Cliente ID {client_id} ERRO: {e}"
        )


def executar_teste(numero_clientes):

    resultados = []

    log("")
    log("=" * 70)
    log(f"INICIO TESTE UDP - {numero_clientes} CLIENTES")
    log("=" * 70)

    threads = []

    inicio = time.perf_counter()

    for client_id in range(1, numero_clientes + 1):

        thread = threading.Thread(
            target=cliente_udp,
            args=(client_id, resultados)
        )

        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()

    fim = time.perf_counter()

    tempo_total = (fim - inicio) * 1000

    log("")
    log(
        f"FIM TESTE UDP - {numero_clientes} CLIENTES"
    )

    log(
        f"Tempo total do teste: "
        f"{tempo_total:.3f} ms"
    )

    if resultados:

        por_comando = {}

        for comando, tempo in resultados:

            if comando not in por_comando:
                por_comando[comando] = []

            por_comando[comando].append(tempo)

        log("")

        for comando in [
            "CREATE",
            "GET",
            "SET",
            "LIST",
            "RESERVE",
            "RELEASE"
        ]:

            tempos = por_comando.get(comando, [])

            if tempos:

                media = sum(tempos) / len(tempos)
                minimo = min(tempos)
                maximo = max(tempos)

                log(
                    f"{comando}: "
                    f"clientes={len(tempos)} | "
                    f"media={media:.3f} ms | "
                    f"min={minimo:.3f} ms | "
                    f"max={maximo:.3f} ms"
                )

    log("=" * 70)
    log("")


def main():

    os.makedirs("logs", exist_ok=True)

    with open(LOG_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("BENCHMARK DE DESEMPENHO - UDP")
    log("")

    for numero_clientes in CLIENTES:

        executar_teste(numero_clientes)

        time.sleep(2)


if __name__ == "__main__":
    main()
