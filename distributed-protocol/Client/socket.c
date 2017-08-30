#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "socket.h"

int create_client(char * ip_server, unsigned int port){ 

	/* E' necessario inizializzare il client, utilizzo la struttura hp per inizializzare!*/

	/*Creo socket
	 * SOCK_STREAM: Connessione sequenzia (TCP)
    * SOCK_DGRAM: Connessione trmite datagramma (UDP)
    * SOCK_RAW: Connessione tramite protocollo IP
	 */

	/*Famiglia Socket:
	 * AF_INET: Protocolli ARPA di internet (TCP, UDP).
    * AF_UNIX: Protocolli interni di sistemi POSIX.
    * AF_NS: Protocolli di Xerox Network System.
    * AF_ISO: Protocolli della International Standard Association.
	 */

	int sd;
	struct sockaddr_in client;
	struct hostent *hp;
	//unsigned short port = PORT;

	hp = gethostbyname(ip_server);

	bzero(&client, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	client.sin_addr.s_addr =((struct in_addr*)(hp->h_addr))->s_addr;

	/* Creo il socket */

	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			logger(unable_create_socket);
			exit(-1);
	}
	   
	/* Mi connetto al server */

	if (connect(sd, (struct sockaddr *)&client, sizeof(client)) < 0){
		logger(unable_connect);
		exit(-1);
	}

	char mess[ADD_LENGHT];
	sprintf(mess,"%d%s%d",port,make_connect_add,sd);
	compose_mess_to_log(make_connect, mess, STRING_TYPE);

	return sd;
}

/*
	Scrive sul socket il buffer passato come parametro.
*/
int write_to_socket(int sd,char * buffer){

	int bytes = send(sd, buffer, strlen(buffer)+1, 0);

	if(bytes < 0 )
		//scrivo errore sul log
		logger(mess_write_socket_error);

	else{
		//scrivo operazione sul log
		//Aggiungo anche testo del messaggio inviato
		char mess[ADD_LENGHT];
		sprintf(mess,"%d%s%s",bytes,mess_write_socket_plus,buffer);
		compose_mess_to_log(mess_write_socket, mess, STRING_TYPE);
	}	

	return bytes;
}  

/*
	Legge sul socket e deposita sul buffer passato come parametro
*/
int read_from_socket(int sd,char *buffer, int type_read){
	
	int bytes = recv(sd, buffer, MAX_MESS_SOCKET_LENGHT, 0);

	if(bytes < 0)
		//scrivo errore sul log
		logger(mess_read_socket_error);

	else{
		//scrivo errore sul log
		//Aggiungo anche testo del messaggio inviato
		if(type_read == PROTOCOL){
			char mess[ADD_LENGHT];
			sprintf(mess,"%d%s%s",bytes,mess_read_socket_plus,buffer);
			compose_mess_to_log(mess_read_socket, mess, STRING_TYPE);
		}
		else{
			char mess[ADD_LENGHT];
			sprintf(mess,"%d",bytes);
			compose_mess_to_log("Download un file, bytes letti: ", &bytes, INTEGER_TYPE);
		}
	}

	return bytes;
}



