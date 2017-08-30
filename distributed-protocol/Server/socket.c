#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "socket.h"

int create_server(int port, int max_call){ 

	int sd;

	/*******************************************************
    * QUESTA PARTE VIENE ESEGUITA ALLO START-UP DEL SERVER *
    *
	 * Creo il socket e riempio i campi della struttura
    * server
    *******************************************************/

	 /*Creo socket
	 * SOCK_STREAM: Connessione sequenzia (TCP)
    * SOCK_DGRAM: Connessione trmite datagramma (UDP)
    * SOCK_RAW: Connessione tramite protocollo IP
	 */
    if ( (sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		  logger(unable_create_socket);
        //printf("Errore nella creazione del server!\n");
    
	 compose_mess_to_log(make_socket,&sd,INTEGER_TYPE);

	 /*Famiglia Socket:
	 * AF_INET: Protocolli ARPA di internet (TCP, UDP).
    * AF_UNIX: Protocolli interni di sistemi POSIX.
    * AF_NS: Protocolli di Xerox Network System.
    * AF_ISO: Protocolli della International Standard Association.
	 */
    server.sin_family = AF_INET;

	 /*Mi metto in ascolto sulla porta passata come parametro*/
    server.sin_port = htons(port);
     
	 /*HOST IP,
	 * Permette al server di essere eseguito senza conoscere l'IP della macchina che lo ospita
	 */
    server.sin_addr.s_addr = INADDR_ANY;
    /* assegnamo al socket un processo tramite la funzione BIND */

	 /*char buff_tmp[20];
	 sprinf(buff_tmp,"%d",MAX_CALL);
	 strcat(mess_read_socket,buff_tmp);
	// strcat(make_bind,MAX_CALL);
	 logger(make_bind);*/
	
	 //int max_call = MAX_CALL;

	 compose_mess_to_log(make_bind,&port, INTEGER_TYPE);
	 compose_mess_to_log(make_bind_conn,&max_call, INTEGER_TYPE);

	 /*Creo il TSAP (IP + PORTA utilizzando TCP)*/
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0){
		  logger(unable_make_bind);	
			return -1;
	}
        //printf("Errore nella chiamata di sistema BIND!\n");
    
    /* In ascolto, massimo connessioni passate come parametro */
    listen (sd, max_call);

	return sd;

}

/*
	Esegue la chiamata di sistema accept per 
*/
int create_client_channel(int sd){

	 if ((client_sd = accept(sd, (struct sockaddr *)&client, &address_size)) < 0) {
		//perror("Errore nella chiamata ACCEPT\n");
		logger(unable_accept);
		exit(-1);
	}

	compose_mess_to_log(make_accept, &client_sd, INTEGER_TYPE);

	return client_sd;
}

/*
	Scrive sul socket il buffer passato come parametro.
*/
int write_to_socket(int sd,char * buffer, int type_write,int nbytes){

	int bytes;

//	if(type_write == PROTOCOL)
	//	bytes = send(sd, buffer, bytes, 0);
//	if(type_write == FILE_BYTES)
		bytes = send(sd, buffer, nbytes, 0);

	if(bytes < 0 )
		//scrivo errore sul log
		logger(mess_write_socket_error);

	else{

		if(type_write == PROTOCOL){
			//scrivo operazione sul log
			//Aggiungo anche testo del messaggio inviato
			char mess[ADD_LENGHT];
			sprintf(mess,"%d%s%s",bytes,mess_write_socket_plus,buffer);
			compose_mess_to_log(mess_write_socket, mess, STRING_TYPE);
		}

		else{
			char mess[ADD_LENGHT];
			sprintf(mess,"%d",bytes);
			compose_mess_to_log("Download file, ho scritto sul socket N bytes: ", &bytes, INTEGER_TYPE);
		}
	}	

	return bytes;
}  

/*
	Legge sul socket e deposita sul buffer passato come parametro
*/
int read_from_socket(int sd,char *buffer){
	
	int bytes = recv(sd, buffer, sizeof(buff), 0);

	if(bytes < 0)
		//scrivo errore sul log
		logger(mess_read_socket_error);

	else{
		//scrivo errore sul log
		//Aggiungo anche testo del messaggio inviato
		char mess[ADD_LENGHT];
		sprintf(mess,"%d%s%s",bytes,mess_read_socket_plus,buffer);
		compose_mess_to_log(mess_read_socket, mess, STRING_TYPE);
	}

	return bytes;
}



