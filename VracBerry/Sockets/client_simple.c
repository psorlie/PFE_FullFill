#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT_DU_SERVEUR (12345)

int main (void)
{
  int un_socket;
  struct sockaddr_in adresse_du_serveur;
  char donnees;

  /* on choisit un socket TCP (SOCK_STREAM) sur IP (PF_INET) */
  un_socket = socket (PF_INET, SOCK_STREAM, 0);
  /* AF_INET = famille TCP/IP */
  adresse_du_serveur.sin_family = AF_INET;
  /* port du serveur auquel se connecter */
  adresse_du_serveur.sin_port = htons (PORT_DU_SERVEUR);
  /* adresse IP (ou nom de domaine) du serveur auquel se connecter */
  adresse_du_serveur.sin_addr = *((struct in_addr *)gethostbyname ("127.0.0.1")->h_addr_list[0]);
  /* On demande la connexion auprès du serveur */
  connect (un_socket, (struct sockaddr *)&adresse_du_serveur, sizeof (adresse_du_serveur));

  read (un_socket, &donnees, 1);
  printf ("Donnee recue : %c\nTaille : %d\n", donnees, 1);

  close (un_socket);

  return 0;
}
