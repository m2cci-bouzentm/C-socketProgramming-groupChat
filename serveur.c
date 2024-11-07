/******************************************************************************/
/*			Application: Dialogue au clavier         */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  Bouzentouta Mohamed, Tarek Ghalleb				      */
/*		Date :  11/07/2024						      */
/*									      */
/******************************************************************************/

#include <stdio.h>

#include <curses.h>
#include <sys/signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <pthread.h>

#include "fon.h" /* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define MAX_CLIENTS 50
#define MAX_REQUESTS 10
#define REQUEST_BUFFER_SIZE 1024

void *recvAndBroadcastData(void *args);
struct AcceptedSocket *accepteConnection(int serverSocketFD);
void AccepteNewConnectionAndRecBroadcastItsData(int serverSocketFD);
void serveur_appli(char *service);

struct AcceptedSocket
{
	int socketFD;
	struct sockaddr_in sock_addr;
	int error;
};

// global variables to track connected clients and their count
int connectedClients[MAX_CLIENTS];
int connectedClientsCount = 0;


int main(int argc, char *argv[])
{

	char *service = SERVICE_DEFAUT; /* numero de service par defaut */

	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
	{
	case 1:
		printf("defaut service = %s\n", service);
		break;
	case 2:
		service = argv[1];
		break;

	default:
		printf("Usage:serveur service (nom ou port) \n");
		exit(1);
	}

	/* service est le service (ou numero de port) auquel sera affecte
	ce serveur*/

	serveur_appli(service);
}

void *recvAndBroadcastData(void *args)
{
	// get the clientSocket from the args of the thread
	struct AcceptedSocket *clientSocket = (struct AcceptedSocket *)args;

	char *buffer = malloc(sizeof(char) * REQUEST_BUFFER_SIZE);
	while (true)
	{
		// wait for client to receive the message
		ssize_t charReceived = recv(clientSocket->socketFD, buffer, REQUEST_BUFFER_SIZE, 0);
		if (charReceived < 0)
		{
			printf("Error receiving from socket : %d\n", clientSocket->socketFD);
			break;
		}

		printf("================== Reading the msg ==================\n");
		printf("%s\n", buffer);

		printf("================== broadcasting msg ==================\n");
		// sending the message to all connected clients except the original sender
		for (int i = 0; i < connectedClientsCount; i++)
		{
			if (connectedClients[i] != clientSocket->socketFD)
			{
				printf("Sending to client %d\n", connectedClients[i]);
				send(connectedClients[i], buffer, REQUEST_BUFFER_SIZE, 0);
			}
		}
		printf("================== End reading / Broadcasting ==================\n");
	}

	close(clientSocket->socketFD);
	free(buffer);
	free(clientSocket);
}

struct AcceptedSocket *accepteConnection(int serverSocketFD)
{
	printf("===== WAITING FOR CONNECTION =====");
	// waiting for connection
	struct sockaddr_in clientAddresse;
	int clientSocketFD = h_accept(serverSocketFD, &clientAddresse);

	// populate the acceptedSocket structure and return to use it in the rest of the program
	struct AcceptedSocket *acceptedSocket = malloc(sizeof(struct AcceptedSocket));
	acceptedSocket->socketFD = clientSocketFD;
	acceptedSocket->sock_addr = clientAddresse;
	if (acceptedSocket->socketFD < 0)
	{
		acceptedSocket->error = clientSocketFD;
	}

	return acceptedSocket;
}

void AccepteNewConnectionAndRecBroadcastItsData(int serverSocketFD)
{
	while (true)
	{
		// accepting upcoming connections on the main thread
		struct AcceptedSocket *clientSocket = accepteConnection(serverSocketFD);

		// registre new connected client/socket
		connectedClients[connectedClientsCount] = clientSocket->socketFD;
		connectedClientsCount++;

		// handle receving / sending data to clients in a seperate thread for each clientSocket
		pthread_t id;
		pthread_create(&id, NULL, recvAndBroadcastData, clientSocket);
	}
}

void serveur_appli(char *service)
{
	// Error handling done by the fon.c library / Thank you Professor !
	int serverSocketFD = h_socket(AF_INET, SOCK_STREAM);
	struct sockaddr_in *p_sock_addr = malloc(sizeof(struct sockaddr_in));

	// calling the adr_socket to populate p_sock_addr for us
	// NULL here refers to 0.0.0.0,
	// 		so will be listening to all up coming request no matter the IP addresses
	adr_socket(service, NULL, SOCK_STREAM, &p_sock_addr);
	// binding local address to a socket with a specific
	// 		local IP address (0.0.0.0 in this case) and port number
	h_bind(serverSocketFD, p_sock_addr);

	// listening to upcoming requests
	// 10 requetes maximum dans la file d'attente
	// handling a maximum of 50 clients at the same time
	h_listen(serverSocketFD, MAX_REQUESTS);

	// start accepting upcoming connections on the main thread
	AccepteNewConnectionAndRecBroadcastItsData(serverSocketFD);

	free(p_sock_addr);
	shutdown(serverSocketFD, SHUT_RDWR);
}
