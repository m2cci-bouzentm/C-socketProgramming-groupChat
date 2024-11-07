/******************************************************************************/
/*			Application: Dialogue au clavier         */
/******************************************************************************/
/*									      */
/*			 programme  CLIENT 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  Bouzentouta Mohamed, Tarek Ghalleb				      */
/*		Date :  11/07/2024						      */
/*									      */
/******************************************************************************/

#include <stdio.h>
#include <curses.h> /* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "fon.h" /* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"
#define REQUEST_BUFFER_SIZE 1024

void send_message(int socketFD, struct sockaddr_in *p_serv_addr);
void *receive_messages(void *args);
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
	char *buffer = malloc(sizeof(char) * REQUEST_BUFFER_SIZE);
	char *message = NULL;
	char *name = NULL;
	unsigned long messageSize = 0;
	unsigned long nameSize = 0;

	// ask the client for his name
	printf("Enter your name : ");
	int nameCount = getline(&name, &nameSize, stdin);
	name[nameCount - 1] = 0; // removing \n charctere from the string

	// start listening for client's requests/messages
	while (true)
	{
		int charCount = getline(&message, &messageSize, stdin);
		// join the name and the message
		sprintf(buffer, "%s: %s", name, message);

		if (charCount > 0)
		{
			if (strcmp(message, "exit") == 0)
			{
				break;
			}
			h_sendto(socketFD, buffer, REQUEST_BUFFER_SIZE, p_serv_addr);
		}
	}

	free(buffer);
	close(socketFD);
	exit(0);
}

void *receive_messages(void *args)
{
	// get the socketFD from the args of the thread
	int socketFD = *((int *)args);
	char *buffer = malloc(sizeof(char) * REQUEST_BUFFER_SIZE);
	while (true)
	{
		ssize_t charReceived = recv(socketFD, buffer, REQUEST_BUFFER_SIZE, 0);
		if (charReceived < 0)
		{
			printf("Error receiving from socket : %d\n", socketFD);
			break;
		}

		// if no error print the received message
		printf("\n%s", buffer);
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