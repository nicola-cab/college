#include <stdio.h>
#include <string.h>

#include "message.h"

/*MESSAGGI DI LOG*/
/***************************************************************************************************************************************************/

char * start = "Server started";
char * end = "Server stopped";
char * unable_create_socket = "Impossibile creare socket, Errore nella creazione del server!";
char * unable_make_bind = "Impossible eseguire la chiamata di sistema BIND";
char * unable_accept = "Impossibile accettare connessioni dal client, Errore nella chiamata di sistema ACCEPT";
char * unable_fork = "Impossibile creare un nuovo processo ERRORE nella FORK, impossibile gestire la chiamata"; 
char * startup_server = "Inizio fase di startup del server";
char make_socket[USEFULL_CONST] = "Creazione del socket, CHANNEL = ";
char make_accept[USEFULL_CONST] = "Nuova richiesta di connessione, esecuzione dell'accept, CHANNEL = ";
char make_accept_add[USEFULL_CONST] = " , client IP = ";
char make_bind[USEFULL_CONST] ="Eseguo la bind, in ascolto sulla porta = ";
char make_bind_conn[USEFULL_CONST] = "Massimo numero di connessioni contemporanee = ";
char mess_write_socket[USEFULL_CONST] = "Scrittura sul socket, BYTES = ";
char mess_read_socket[USEFULL_CONST] = "Lettura dal socket, BYTES = ";
char mess_write_socket_plus[USEFULL_CONST] = " , MESSAGE = ";
char mess_read_socket_plus[USEFULL_CONST] = ", MESSAGE = ";
char *mess_write_socket_error = "Errore scrittura sul socket";
char *mess_read_socket_error = "Errore lettura dal socket";
char close_socket[USEFULL_CONST] = "Chiusura del socket, SOCKET_DESCRIPTOR = ";

/***************************************************************************************************************************************************/



/*
Compone un messaggio con dei dati a runtime.
Utilizzo un puntatore di tipo void per poter passare 
interi, stringhe, o qualsiasi altro tipo di dato da 
castare correttamente.
Il parametro "tipo" viene utilizzato per castare a stringa, intero, float
Stringa: tipo = 0;
Intero: tipo = 1;
Float: tipo = 2;
*/

void compose_mess_to_log(char * str, void * ptr, int tipo){

	//debug
	//logger("compongo messaggio di log");

	char buff_tmp[ADD_LENGHT];
	char str_tmp[USEFULL_CONST];

	//inutile
	bzero(buff_tmp,ADD_LENGHT);
	bzero(str_tmp,USEFULL_CONST);

	strcpy(str_tmp,str);
	
	if(tipo == INTEGER_TYPE){
		int  *dato = (int *) ptr;
		sprintf(buff_tmp,"%d",*dato);
	}	
	else 	if(tipo == STRING_TYPE){
		char * dato = (char *) ptr;
		sprintf(buff_tmp,"%s",dato);	
	}
	else	if (tipo == FLOAT_TYPE){
		//Unused at moment
		float * dato = (float *) ptr;
		sprintf(buff_tmp,"%f",*dato);	
	}	
	else
		return; //Nothing per il momento

	//debug
	//logger("scrivo messaggio di log");

	strcat(str_tmp,buff_tmp);
	logger(str_tmp);

	//free(buff_tmp);
	//free(str_tmp);

return;

}

