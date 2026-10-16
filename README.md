# Wikicomptines - TCP Client-Server in C

Wikicomptines is a university project developed in 2024 during my second year of a Bachelor's degree in Computer Science at Université Sorbonne Paris Nord.

The project consists of implementing a client-server application in C that allows users to browse and retrieve nursery rhymes stored on a server through a terminal.

Communication uses TCP over IPv4 and follows a custom application-layer protocol called **WCP (Wikicomptines Protocol)**.

## Features

- **Nursery rhyme catalog:** Load `.cpt` files and organize their contents in memory.
- **TCP server:** Listen for incoming client connections and provide access to the catalog.
- **TCP client:** Connect to a server using an IPv4 address.
- **Catalog listing:** Retrieve and display the available nursery rhymes.
- **Nursery rhyme retrieval:** Request a rhyme using its catalog index and display its contents.
- **Network byte order:** Exchange the selected rhyme index as a 16-bit unsigned integer in network byte order.
- **Memory management:** Allocate and release resources used to manage rhymes and the catalog.

## Technologies

- C
- TCP/IP and IPv4 sockets
- Client-server architecture
- Application-layer protocol design
- File and directory operations
- Dynamic memory allocation
- GNU Make

## Project Structure

```
wikicomptines/
├── include/
│   └── comptine_utils.h
├── src/
│   ├── comptine_utils.c
│   ├── wcp_clt.c
│   └── wcp_srv.c
├── comptines/
│   └── *.cpt
├── Makefile
├── .gitignore
├── LICENSE
├── Rapport.pdf
└── README.md
```

## Requirements

- GCC
- GNU Make
- A Unix-like environment, such as Linux

## Build

Clone the repository:

```bash
git clone git@github.com:daizerbercy/wikicomptines-c-tcp-sever.git
cd wikicomptines-c-tcp-sever
```

Compile the project:

```bash
make
```
## Run

### 1. Start the server

In the first terminal, run:

```bash
./wcp_srv comptines
```

The server starts listening for incoming connections:

```
Serveur en attente de connexions...
```

### 2. Start the client

In a second terminal, run:

```bash
./wcp_clt 127.0.0.1
```

The client connects to the server on port `4321` and displays the available nursery rhymes:

```
Connexion établie avec le serveur 127.0.0.1:4321
     0 Mon petit lapin
     1 Dans sa maison, un grand cerf
     2 Petit escargot
     3 La famille tortue
     4 Les petits poissons dans l'eau
     5 G comme Gaston
Quelle comptine voulez-vous ?(Entrer un entier entre 0 et 5) :
```

### 3. Request a nursery rhyme

Enter `2` to retrieve _Petit escargot_:

```
Quelle comptine voulez-vous ?(Entrer un entier entre 0 et 5) : 2
Petit escargot

Petit escargot
Porte sur son dos
Sa maisonnette
Aussitôt qu'il pleut
Il est tout heureux
Il sort sa tête
```

The server reports the request in its terminal:

```
Serveur en attente de connexions...
Connexion acceptée
liste envoyée
comptine envoyée
```

This example demonstrates the main client-server interaction: establishing a TCP connection, receiving the catalog, selecting a nursery rhyme and retrieving its contents.

## Communication Protocol

The application uses a custom protocol called WCP over TCP.

The main exchange follows these steps:

1. The client establishes a connection with the server.
2. The server sends the list of available nursery rhymes.
3. The client displays the catalog and prompts the user to select a rhyme.
4. The client sends the selected catalog index as a 16-bit unsigned integer in network byte order.
5. The server sends the requested rhyme.
6. The end of the rhyme is indicated by the protocol's terminating blank lines.

The server listens on TCP port `4321`.

## Project Scope

This repository contains the core client-server functionality implemented for the original university assignment.

Optional advanced features described in the assignment, such as multithreading, request logging and remote file uploads, are not included in this implementation.

## Academic Context

- **Institution:** Université Sorbonne Paris Nord
- **Program:** Bachelor's degree in Computer Science - Second year
- **Course:** Systems and Networks
- **Academic year:** 2023-2024
- **Project type:** Individual university project

