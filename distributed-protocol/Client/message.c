#include <stdio.h>
#include <string.h>

#include "message.h"

/*MESSAGGI DI LOG*/
/***************************************************************************************************************************************************/

char * start = "Client started";
char * end = "Client stopped";
char * unable_create_socket = "Impossibile creare socket, Errore nella creazione del Client!";
char * unable_make_bind = "Impossible eseguire la chiamata di sistema BIND";
char * unable_connect = "Impossibile connettersi al server";
char * unable_fork = "Impossibile creare un nuovo processo ERRORE nella FORK, impossibile gestire la chiamata"; 
char * startup_client = "Inizio fase di startup del client";
char make_socket[USEFULL_CONST] = "Creazione del socket, CHANNEL = ";
char make_connect[USEFULL_CONST] = "Esecuzione della connect sulla PORTA = ";
char make_connect_add[USEFULL_CONST]= " , CHANNEL = ";
//char make_bind[USEFULL_CONST] ="Eseguo la bind, in ascolto sulla porta 9000, MAX_CONN_CONTEMP = ";
char mess_write_socket[USEFULL_CONST] = "Scrittura sul socket, BYTES = ";
char mess_read_socket[USEFULL_CONST] = "Lettura dal socket, BYTES = ";
char mess_write_socket_plus[USEFULL_CONST] = " , MESSAGE = ";
char mess_read_socket_plus[USEFULL_CONST] = ", MESSAGE = ";
char *mess_write_socket_error = "Errore scrittura sul socket";
char *mess_read_socket_error = "Errore lettura dal socket";
char close_socket[USEFULL_CONST] = "Chiusura del socket, SOCKET_DESCRIPTOR = ";
char * client_missing_param = "Manca l'indirizzo IP del server, mancanza di parametri nel main";
char * client_format_param_error = "Formato indirizzo IP non corretto";
char * unable_draw = "Impossibile leggere il file contenente il template per il menu' principale (menu.txt)";

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
	//logger("Sto per scivere con la compose mess");

	char buff_tmp[ADD_LENGHT];
	//reset del buffer
	char str_tmp[USEFULL_CONST];
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
	//logger("Sto per scivere con la compose mess");

	strcat(str_tmp,buff_tmp);
	logger(str_tmp);
//logger("FINE return");
return;

}

