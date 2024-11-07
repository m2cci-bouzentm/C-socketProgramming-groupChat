/******************************************************************************/
/*			Application: Dialogue au clavier					*/
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : Bouzentouta Mohamed 					*/
/*									      */
/******************************************************************************/

#include <stdio.h>
#include <curses.h> /* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "fon.h" /* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"
#define REQUEST_BUFFER_SIZE 1024

void client_appli(char *serveur, char *service);

/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur = SERVEUR_DEFAUT; /* serveur par defaut */
	char *service = SERVICE_DEFAUT; /* numero de service par defaut (no de port) */

	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
	{
	case 1: /* arguments par defaut */
		printf("serveur par defaut: %s\n", serveur);
		printf("service par defaut: %s\n", service);
		break;
	case 2: /* serveur renseigne  */
		serveur = argv[1];
		printf("service par defaut: %s\n", service);
		break;
	case 3: /* serveur, service renseignes */
		serveur = argv[1];
		service = argv[2];
		break;
	default:
		printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */

	client_appli(serveur, service);
}

/*****************************************************************************/
void send_message(int socketFD, struct sockaddr_in *p_serv_addr)
{
	unsigned long lineSize = 0;
	char *message = malloc(sizeof(char) * REQUEST_BUFFER_SIZE);
	while (true)
	{
		printf("Write a message : ");
		long charCount = getline(&message, &lineSize, stdin);
		if (charCount > 0)
		{
			if (strcmp(message, "exit\n") == 0)
			{
				break;
			}
			h_sendto(socketFD, message, REQUEST_BUFFER_SIZE, p_serv_addr);
		}
	}

	free(message);
	close(socketFD);
	exit(0);
}

void *receive_messages(void *args)
{
	int *s = (int *)args;
	int socketFD = *s;
	char *buffer = malloc(sizeof(char) * REQUEST_BUFFER_SIZE);
	while (true)
	{
		ssize_t charReceived = recv(socketFD, buffer, REQUEST_BUFFER_SIZE, 0);
		// ssize_t charReceived = h_recvfrom(socketFD, buffer, REQUEST_BUFFER_SIZE, p_serv_addr);
		if (charReceived > 0)
		{
			// buffer[charReceived] = 0;
			printf("Server : %s\n", buffer);
		}
		else if (charReceived == 0)
			break;
		else
		{
			printf("Error receiving from socket : %d\n", socketFD);
			break;
		}
	}

	free(buffer);
	close(socketFD);
}

void client_appli(char *serveur, char *service)
{

	int socketFD = h_socket(AF_INET, SOCK_STREAM);
	// Pointeur sur les infos adresse interlocuteur
	struct sockaddr_in *p_serv_addr = malloc(sizeof(struct sockaddr_in));
	adr_socket(service, serveur, SOCK_STREAM, &p_serv_addr);
	h_connect(socketFD, p_serv_addr);

	// listen to incoming messages in separate thread as it's code blocking
	pthread_t id;
	pthread_create(&id, NULL, receive_messages, (void *)&socketFD);

	// send messages in the main thread
	send_message(socketFD, p_serv_addr);
}