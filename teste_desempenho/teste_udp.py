import socket
import threading
import time
import os
from datetime import datetime

HOST = "127.0.0.1"
PORT = 8081

USUARIO = "andreia"
SENHA = "1234"

NUM_CLIENTES = [5, 10, 20, 50, 100]

LOG_FILE = "logs/desempenho_udp.log"

log_lock = threading.Lock()


def agora():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]


def salvar_log(linhas):
    with log_lock:
        with open(LOG_FILE, "a", encoding="utf-8") as f:
            for linha in linhas:
                f.write(linha + "\n")


def enviar(sock, endereco, client_id, comando, linhas, resultados):

    linhas.append(
        f"[{agora()}] Cliente ID {client_id} -> {comando}"
    )

    inicio = time.perf_counter()

    sock.sendto(
        comando.encode(),
        endereco
    )

    resposta, _ = sock.recvfrom(4096)

    resposta = resposta.decode().strip()

    fim = time.perf_counter()

    tempo = (fim - inicio) * 1000

    linhas.append(
        f"[{agora()}] Cliente ID {client_id} <- {resposta}"
    )

    resultados.append((comando.split()[0], tempo))

    return resposta


def cliente(client_id, barreira, resultados_globais):

    linhas = []

    try:

        sock = socket.socket(
            socket.AF_INET,
            socket.SOCK_DGRAM
        )

        # Porta aleatória para diferenciar os clientes
        sock.bind(("127.0.0.1", 0))

        endereco = (HOST, PORT)

        linhas.append(
            f"[{agora()}] Cliente ID {client_id} conectado"
        )

        # Autenticação
        resposta = enviar(
            sock,
            endereco,
            client_id,
            f"LOGIN {USUARIO} {SENHA}",
            linhas,
            []
        )

        if not resposta.startswith("OK"):

            linhas.append(
                f"[{agora()}] Cliente ID {client_id} "
                f"falha na autenticacao"
            )

            salvar_log(linhas)
            sock.close()
            return

        # Espera os demais clientes
        barreira.wait()

        recurso = f"bench_udp_{client_id}"

        resultados = []

        # CREATE
        enviar(
            sock,
            endereco,
            client_id,
            f"CREATE {recurso} 100",
            linhas,
            resultados
        )

        # GET
        enviar(
            sock,
            endereco,
            client_id,
            f"GET {recurso}",
            linhas,
            resultados
        )

        # SET
        enviar(
            sock,
            endereco,
            client_id,
            f"SET {recurso} 200",
            linhas,
            resultados
        )

        # RESERVE
        enviar(
            sock,
            endereco,
            client_id,
            f"RESERVE {recurso}",
            linhas,
            resultados
        )

        # RELEASE
        enviar(
            sock,
            endereco,
            client_id,
            f"RELEASE {recurso}",
            linhas,
            resultados
        )

        resultados_globais.extend(resultados)

        linhas.append(
            f"[{agora()}] Cliente ID {client_id} desconectado"
        )

        sock.close()

        salvar_log(linhas)

    except Exception as e:

        linhas.append(
            f"[{agora()}] Cliente ID {client_id} ERRO: {e}"
        )

        salvar_log(linhas)


def executar(numero_clientes):

    resultados = []

    salvar_log([
        "",
        "=" * 70,
        f"[{agora()}] INICIO TESTE UDP - {numero_clientes} CLIENTES",
        "=" * 70
    ])

    barreira = threading.Barrier(numero_clientes)

    threads = []

    inicio = time.perf_counter()

    for i in range(1, numero_clientes + 1):

        t = threading.Thread(
            target=cliente,
            args=(i, barreira, resultados)
        )

        threads.append(t)
        t.start()

    for t in threads:
        t.join()

    fim = time.perf_counter()

    tempo_total = (fim - inicio) * 1000

    salvar_log([
        f"[{agora()}] FIM TESTE UDP - {numero_clientes} CLIENTES",
        f"[{agora()}] Tempo total: {tempo_total:.3f} ms"
    ])

    comandos = [
        "CREATE",
        "GET",
        "SET",
        "RESERVE",
        "RELEASE"
    ]

    for comando in comandos:

        tempos = [
            tempo
            for cmd, tempo in resultados
            if cmd == comando
        ]

        if tempos:

            media = sum(tempos) / len(tempos)
            minimo = min(tempos)
            maximo = max(tempos)

            salvar_log([
                f"[{agora()}] {comando}: "
                f"media={media:.3f} ms | "
                f"min={minimo:.3f} ms | "
                f"max={maximo:.3f} ms"
            ])

    salvar_log([
        "=" * 70,
        ""
    ])


def main():

    os.makedirs("logs", exist_ok=True)

    with open(LOG_FILE, "w", encoding="utf-8") as f:
        f.write("")

    salvar_log([
        f"[{agora()}] BENCHMARK DE DESEMPENHO - UDP"
    ])

    for numero_clientes in NUM_CLIENTES:

        executar(numero_clientes)

        time.sleep(1)


if __name__ == "__main__":
    main()
