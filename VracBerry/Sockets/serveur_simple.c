#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_PENDING_CONNECTIONS (5)

#define PORT_DU_SERVEUR (12345)

void communication_avec_client (int socket)
{
  char ma_donnee = 'A';
  int quantite_envoyee;

  //TODO : Construction du JSON

  /* on envoie une donnee jusqu'à ce que le client ferme la connexion */
  quantite_envoyee = write (socket, &ma_donnee, 1);
  //quantite_envoyee = write (socket, &ma_donnee, sizeof (ma_donnee));
  printf("Nombre de caractères envoyés : %d\n", quantite_envoyee);

  /* la connexion est normalement déjà fermée par le client, mais la prudence ne coûte rien */
  close (socket);

  exit (0);	
}

int main (void)
{


  /* ESSAI
  char* ma_donnee = "Helloworld";
  printf("Chaine : %s, nombre de caractères : %d\n", ma_donnee, sizeof (ma_donnee));
  */

  int socket_ecoute;
  int socket_donnees;
  struct sockaddr_in mon_adresse;

  /************ Creation du socket : PF_INET = IP, SOCK_STREAM = TCP ***************/
  socket_ecoute = socket (AF_INET, SOCK_STREAM, 0);
  if(socket_ecoute == -1)
  {
    perror("socket()");
    exit(errno);
  }

  /************ Adresse à laquelle on écoute **************/
  mon_adresse.sin_family = AF_INET; /* Type d'adresse = IP */
  mon_adresse.sin_port = htons (PORT_DU_SERVEUR); /* Port TCP ou le service est accessible */
  //mon_adresse.sin_addr.s_addr = htonl (INADDR_ANY); /* On s'attache a toutes les interfaces */
  mon_adresse.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
  //mon_adresse.sin_addr.s_addr = htonl(*gethostbyname ("127.0.0.1")->h_addr_list[0]);

  /* On attache le socket a l'adresse indiquee */
  if(bind (socket_ecoute, (struct sockaddr *)&mon_adresse, sizeof (mon_adresse)) == -1){
    perror("bind()");
    exit(errno);
  }

  /************** Mise en ecoute du socket ***************/
  printf("en attente de connexion...\n");
  listen (socket_ecoute, MAX_PENDING_CONNECTIONS);

  while (1)
  {
    printf("handshake...\n");
    /**************** Acceptation de la connexion **************/
    socket_donnees = accept (socket_ecoute, NULL, 0);
    printf("connexion acceptée...\n");

    /************** On cree une tache qui va communiquer avec le client ************/
    if (fork () == 0){
      printf("envoi de données...\n");
      communication_avec_client (socket_donnees);
    }
  }

  /************* On ferme le port sur lequel on ecoutait **************/
  close (socket_ecoute);
}
