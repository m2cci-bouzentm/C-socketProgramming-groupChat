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

#include "fon.h" /* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

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
void client_appli(char *serveur, char *service)
{

	int socketFD = h_socket(AF_INET, SOCK_STREAM);

	// Pointeur sur les infos adresse interlocuteur
	// struct sockaddr_in* p_serv_addr;
	struct sockaddr_in *p_serv_addr = malloc(sizeof(struct sockaddr_in));
	adr_socket(service, serveur, SOCK_STREAM, &p_serv_addr);
	h_connect(socketFD, p_serv_addr);

	char *message;
	message = "Hello From Client\n";
	h_sendto(socketFD, message, 1024, p_serv_addr);

	char *response = malloc(sizeof(char) * 1024);
	h_recvfrom(socketFD, response, 1024, p_serv_addr);

	printf("====================================================\n");
	printf("Server : %s\n", response);
	printf("====================================================\n");
}