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

#include "fon.h" /* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"

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
	h_listen(socketFD, 1000);

	// waiting for connection
	int reqNumber = 1;
	while (true)
	{
		printf("====================================================\n");
		printf("===== WAITING FOR CONNECTION =====");

		// accepte
		int new_socket = h_accept(socketFD, p_sock_addr);

		char *request = malloc(sizeof(char) * 1024);
		h_reads(new_socket, request, 1024);
		printf("====================================================\n");
		printf("Client : %s\n", request);
		printf("====================================================\n");

		//  respond
		char *response = "Hello, World!";
		h_writes(new_socket, response, 1024);
		h_close(new_socket);

		printf("\n%d\n", reqNumber);
		reqNumber++;
	}
}