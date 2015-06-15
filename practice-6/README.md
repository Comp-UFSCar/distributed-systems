# Sistemas Distribuidos - pratica 6

## Introdução
Exemplo de sincronia distribuída utilizando um serviço sincronizador escrito em uma linguagem diferente dos programas de mensagem.

O repositório do projeto pode ser [encontrado aqui](https://github.com/lucasdavid/distributed-systems/tree/master/practice-6).

### Alunos
* Lucas Oliveira David 407917

## Execução
O projeto pode ser executado da seguinte maneira:

* Compile os projetos `client` e `server` normalmente, pelo Visual Studio.
* Execute o servidor de mensagens: `Debug\server.exe`.
* Execute o servidor de sincronização: `python3 sync-server\sync_server.py`.
* Execute múltiplos clientes: `Debug\client.exe`.
