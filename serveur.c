/******************************************************************************/
/*			Application: Dialogue au clavier         */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  Bouzentouta Mohamed				      */
/*		Date :  11/05/2024						      */
/*									      */
/******************************************************************************/

#include <stdio.h>

#include <curses.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <sys/mman.h>
#include <errno.h>

#include "fon.h" /* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define MAX_CLIENTS 50

void serveur_appli(char *service); /* programme serveur */

/******************************************************************************/
/*---------------- programme serveur ------------------------------*/

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

void serveur_appli(char *service)
{
	// Error handling done by the fon.c library / Thank you Professor !
	int socketFD = h_socket(AF_INET, SOCK_STREAM);
	struct sockaddr_in *p_sock_addr = malloc(sizeof(struct sockaddr_in));

	// calling the adr_socket to populate p_sock_addr for us
	// NULL here refers to 0.0.0.0,
	// 		so will be listening to all up coming request no matter the IP addresses
	adr_socket(service, NULL, SOCK_STREAM, &p_sock_addr);

	// binding local address to a socket with a specific
	// 		local IP address (0.0.0.0 in this case) and port number
	h_bind(socketFD, p_sock_addr);

	// listening to upcoming requests
	// 1000 requetes maximum dans la file d'attente
	// handling a maximum of 50 clients at the same time
	h_listen(socketFD, 1000);
	int *connectedSockets = (int *)mmap(
			NULL, MAX_CLIENTS * sizeof(int), PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	int *clientsCount = (int *)mmap(
			NULL, sizeof(int), PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS, -1, 0);

WAITING:
	// waiting for connection
	printf("====================================================\n");
	printf("===== WAITING FOR CONNECTION =====");
	// accepte the connection
	int new_socket = h_accept(socketFD, p_sock_addr);
	connectedSockets[*clientsCount] = new_socket;
	(*clientsCount)++;

	// fork the program if accepting a new request
	pid_t p = fork();

	while (true)
	{
		// handle the request only if received one and created a new thread
		if (p < 0)
		{
			perror("fork fail");
			exit(1);
		}
		if (p > 0)
		{
			printf("I'm The Parent. %d\n", p);
			msync(connectedSockets, MAX_CLIENTS * sizeof(int), MS_SYNC);
			msync(clientsCount, MAX_CLIENTS * sizeof(int), MS_SYNC);
			for (int i = 0; i < *clientsCount; i++)
				fsync(connectedSockets[i]);

			goto WAITING;
		}
		if (p == 0)
		{
			getpid();
			char *request = malloc(sizeof(char) * 1024);
			h_reads(new_socket, request, 1024);
			printf("====================================================\n");
			printf("Client : %s\n", request);
			printf("====================================================\n");

			// 	send the client's message to all other clients except the owner
			for (int i = 0; i < *clientsCount; i++)
				printf("connectedSockets[%d] = %d;\n", i, connectedSockets[i]);

			for (int i = 0; i < *clientsCount; i++)
			{
				// if (connectedSockets[i] != new_socket)
				{
					h_writes(connectedSockets[i], request, strlen(request));
				}
			}

			printf("clients Count : %d\n", *clientsCount);
			free(request);
		}

		// close the connection
		// h_close(new_socket);
	}
}
