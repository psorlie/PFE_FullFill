// Dans ce fichier, il faut modifier les callbacks d'envoi et de réception des données ainsi que le main

#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "uartTestCrossCompil.h"


static struct lws *web_socket = NULL;

#define EXAMPLE_RX_BUFFER_BYTES (4)

static int callback_example( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	switch( reason )
	{
		// Etablissement de la connexion
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			lws_callback_on_writable( wsi );
			break;

		// Réception des données
		case LWS_CALLBACK_CLIENT_RECEIVE:
			// Ici va le JSON parse pour les données recçues.
			// Il doit exister une fonction lws_read un peu comme lws_write( wsi, p, n, LWS_WRITE_TEXT );
			break;

		// Envoi de données
		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			// Patouche aux trois premières lignes
			receiveFormatSend ();
			unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
			unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];

			// Ici appeler la fonction qui contient les infos dont on a besoin
			// En l'occurence c'est get_value pour récupérer ce que le XBee a reçu
			// Garder le même formatage de données pour p et n
			size_t n = get_value((char*)p);
			//size_t n = sprintf( (char *)p, "%u", rand() );
			lws_write( wsi, p, n, LWS_WRITE_TEXT );
			break;
		}

		case LWS_CALLBACK_CLOSED:
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			web_socket = NULL;
			break;

		default:
			break;
	}

	return 0;
}

enum protocols
{
	PROTOCOL_EXAMPLE = 0,
	PROTOCOL_COUNT
};

static struct lws_protocols protocols[] =
{
	{
		"example-protocol",
		callback_example,
		0,
		EXAMPLE_RX_BUFFER_BYTES,
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};

int main( int argc, char *argv[] )
{
	/******************** Patouche à cette section ************************/
	struct lws_context_creation_info info;
	memset( &info, 0, sizeof(info) );

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;

	struct lws_context *context = lws_create_context( &info );
	/******************** Fin de la section patouche ************************/

	/******************** A changer en fonction de l'application ************************/
	time_t old = 0;
	while( 1 )
	{
		struct timeval tv;
		gettimeofday( &tv, NULL );

		// On se connecte si ce n'est pas déjà fait
		if( !web_socket && tv.tv_sec != old )
		{
			// Changer l'adresse et le port d'écoute si nécessaire
			struct lws_client_connect_info ccinfo = {0};
			ccinfo.context = context;
			ccinfo.address = "localhost";
			ccinfo.port = 8000;
			ccinfo.path = "/";
			ccinfo.host = lws_canonical_hostname( context );
			ccinfo.origin = "origin";
			ccinfo.protocol = protocols[PROTOCOL_EXAMPLE].name;
			web_socket = lws_client_connect_via_info(&ccinfo);
		}

		// Go à chaque fois que le timeout est passé
		if( tv.tv_sec != old )
		{
			// Là on appelle la callback d'envoi au serveur
			lws_callback_on_writable( web_socket );
			old = tv.tv_sec;	// on rafraîchit le timer
		}

		lws_service( context, /* timeout_ms = */ 5000 );	// changer le timeout si nécessaire
	}

	lws_context_destroy( context );

	return 0;
}
