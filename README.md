# Serviço de Coordenação Distribuída

Sistema cliente-servidor desenvolvido em C utilizando sockets, com o objetivo de implementar um serviço de coordenação distribuída baseado em um estado compartilhado.

O sistema permite que múltiplos clientes se conectem aos servidores, autentiquem-se e realizem operações de criação, consulta, alteração, reserva e liberação de recursos compartilhados.

---

## Sobre o trabalho

O trabalho foi desenvolvido como parte da disciplina de **MAC5910 - Programação para Redes de Computadores (2026)**, com foco na implementação de conceitos fundamentais de comunicação em redes, incluindo:

- Comunicação cliente-servidor
- Sockets TCP
- Sockets UDP
- Protocolo de aplicação
- Concorrência
- Gerenciamento de estado compartilhado
- Autenticação
- Persistência de dados
- Logging
- Monitoramento
- Replicação de estado

A implementação possui dois modos de comunicação:

1. **Comunicação TCP:** utilizada pelos clientes para acessar o servidor principal, que processa as operações sobre os recursos compartilhados e realiza a replicação das alterações para um servidor réplica.
2. **Comunicação UDP:** disponibilizada como uma alternativa ao TCP, permitindo que clientes realizem as mesmas operações diretamente com o servidor UDP. O servidor UDP funciona de forma independente e não utiliza o servidor réplica.

---

## Arquitetura

A arquitetura do sistema é composta por clientes, um servidor principal TCP, um servidor UDP independente e um servidor réplica.

### Comunicação TCP

No fluxo TCP, os clientes se conectam ao **Servidor Principal**, responsável pelo processamento das requisições, gerenciamento do estado compartilhado, autenticação, persistência, logging e monitoramento.

As operações que modificam o estado dos recursos são encaminhadas também ao **Servidor Réplica**, permitindo manter uma cópia do estado dos recursos.

### Comunicação UDP

O **Servidor UDP** funciona de forma independente do servidor principal TCP. Clientes UDP podem realizar operações sobre os recursos utilizando o protocolo UDP, sem participação do servidor réplica.

```text
                         ┌──────────────────────────┐
                         │         CLIENTES         │
                         │                          │
                         │  Clientes TCP            │
                         │  Clientes UDP            │
                         └────────────┬─────────────┘
                                      │
                       ┌──────────────┴──────────────┐
                       │                             │
                     TCP                           UDP
                       │                             │
                       ▼                             ▼
          ┌─────────────────────────┐     ┌─────────────────────────┐
          │    SERVIDOR PRINCIPAL   │     │      SERVIDOR UDP       │
          │          TCP            │     │                         │
          │                         │     │ • Autenticação          │
          │ • Autenticação          │     │ • Processamento         │
          │ • Processamento         │     │ • Estado dos recursos   │
          │ • Concorrência          │     │ • Persistência           │
          │ • Estado dos recursos   │     │ • Logging               │
          │ • Persistência          │     │ • Monitoramento         │
          │ • Logging               │     └────────────┬────────────┘
          │ • Monitoramento         │                  │
          └────────────┬────────────┘                  │
                       │                               │
                  Replicação                           │
                       │                               │
                       ▼                               ▼
          ┌─────────────────────────┐     ┌─────────────────────────┐
          │    SERVIDOR RÉPLICA     │     │   RECURSOS / ESTADO     │
          │                         │     │       PERSISTIDO         │
          │ • Estado replicado      │     └─────────────────────────┘
          │ • Persistência          │
          └────────────┬────────────┘
                       │
                       ▼
          ┌─────────────────────────┐
          │   RECURSOS / ESTADO     │
          │       PERSISTIDO        │
          └─────────────────────────┘
