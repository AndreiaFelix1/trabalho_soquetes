# Serviço de Coordenação Distribuída

Sistema cliente-servidor desenvolvido em C utilizando sockets, com o objetivo de implementar um serviço de coordenação distribuída baseado em um estado compartilhado.

O sistema permite que múltiplos clientes se conectem ao servidor, autentiquem-se e realizem operações de criação, consulta, alteração, reserva e liberação de recursos compartilhados.

---

## Sobre o projeto

O projeto foi desenvolvido como parte da disciplina de Programação com Soquetes, com foco em conceitos de:

- Comunicação cliente-servidor
- Sockets TCP
- Protocolo de aplicação
- Concorrência
- Gerenciamento de estado compartilhado
- Autenticação
- Persistência
- Logging
- Monitoramento
- Replicação
- Comunicação UDP

A arquitetura principal utiliza um servidor de coordenação, responsável por manter o estado dos recursos compartilhados e processar as requisições dos clientes.

---

## Arquitetura

A arquitetura do sistema é composta por:

```text
                  ┌─────────────────────┐
                  │      Clientes       │
                  │                     │
                  │  Cliente 1          │
                  │  Cliente 2          │
                  │  ...                │
                  │  Cliente N          │
                  └──────────┬──────────┘
                             │
                             │ TCP
                             ▼
                 ┌─────────────────────────┐
                 │    Servidor Principal   │
                 │                         │
                 │  • Autenticação         │
                 │  • Processamento        │
                 │  • Concorrência         │
                 │  • Estado compartilhado │
                 └───────┬─────────┬───────┘
                         │         │
              ┌──────────┘         └──────────┐
              ▼                               ▼
      ┌───────────────┐               ┌───────────────┐
      │  Persistência │               │  Monitoramento │
      └───────────────┘               └───────────────┘
              │
      ┌───────────────┐
      │    Logging    │
      └───────────────┘

                         │
                         │ Replicação
                         ▼
                ┌───────────────────┐
                │ Servidor Réplica  │
                └───────────────────┘
