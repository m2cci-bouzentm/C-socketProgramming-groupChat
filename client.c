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

typedef struct thread_params
{
	int socketFD;
	struct sockaddr_in *p_serv_addr;
	char *message;
	char *response;
} thread_params_t;

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
void *send_message(void *arg)
{
	thread_params_t *params = (thread_params_t *)arg;
	while (true)
	{
		printf("Write a message : ");
		scanf("%s", params->message);
		h_sendto(params->socketFD, params->message, 1024, params->p_serv_addr);
		strcopy(params->message, "");
	}
}
void *receive_messages(void *arg)
{
	thread_params_t *params = (thread_params_t *)arg;
	int taille = sizeof(*params->p_serv_addr);

	while (true)
	{
		// h_recvfrom(params->socketFD, params->response, 1024, params->p_serv_addr);
		recvfrom(params->socketFD, params->response, 1024, 0, (struct sockaddr *)params->p_serv_addr, (socklen_t *)&taille);
		printf("\nServer : %s\n", params->response);
		strcopy(params->response, "");
	}
}

void client_appli(char *serveur, char *service)
{

	int socketFD = h_socket(AF_INET, SOCK_STREAM);
	// Pointeur sur les infos adresse interlocuteur
	struct sockaddr_in *p_serv_addr = malloc(sizeof(struct sockaddr_in));
	adr_socket(service, serveur, SOCK_STREAM, &p_serv_addr);
	h_connect(socketFD, p_serv_addr);
	char *message = malloc(sizeof(char) * 1024);
	char *response = malloc(sizeof(char) * 1024);

	pthread_t send_thread, receive_thread;
	thread_params_t params;
	params.socketFD = socketFD;
	params.p_serv_addr = p_serv_addr;
	params.message = malloc(sizeof(char) * 1024);
	params.response = malloc(sizeof(char) * 1024);

	int create_send_thread = pthread_create(&send_thread, NULL, send_message, &params);
	int create_receive_thread = pthread_create(&receive_thread, NULL, receive_messages, &params);

	if (create_send_thread != 0 || create_receive_thread != 0)
	{
		perror("Failed to a thread");
		exit(1);
	}

	// free(params.p_serv_addr);
	pthread_join(send_thread, NULL);
	pthread_join(receive_thread, NULL);
}