#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#include "op.h"

int main(int argc,char * argv []){

	//nessun session id sul server (no loggato)
	session_id.id = -1;

	logger(startup_client);

	//Prima di eseguira qualsiasi operazione controllo che vengano passati come parametri al programma (da shell)
	//1) IP del SERVER (127.0.0.1) o (localhost)

	//debug
	//printf("Valori : %d - %s - %s\n",argc,argv[0],argv[1]);

	if(argc < 2){
		//manca l'indirizzo del server!!!
		printf("Mancano parametri, indirizzo IP del server");
		logger(client_missing_param);
		exit(-1);
	}

	//In questa versione di prova possiamo testare solo su localhost.. (commentare per altro indirizzo IP)
	/*if((strcmp(argv[1],"localhost")) && (strcmp(argv[1],"127.0.0.1"))){
		printf("Test solo localhost");
		logger(client_format_param_error);
		exit(-1);
	}*/

	//logger("Prima di conv localhost");

	if(!strcmp(argv[1],"localhost"))
		strcpy(argv[1],"127.0.0.1");

	//logger("Dopo di conv localhost");

	//memorizzo in una variabile globale l'ip
	strcpy(ip_server_connect,argv[1]);

	//logger("Passo connect al SO");

	//creo il client
	int sd = create_client(ip_server_connect,PORT);
	logger(start);

	//invoco gestore interfaccia grafica
	build_gui(argc,argv,sd);

	//carico tutti i download vecchi da db
	//execute_select_items(items);

	//disegno il menu sulla shell

	/*int choice;  //identifica la scelta fatta dall'utente
	while(choice != EXIT){

	//Creo un semplice menù dal quale l'utente può scegliere di
	//1)LOGIN (viene richiesto username e password
	//2)SCARICARE UN FILE (viene richiesto il catalogo)
	//3)VEDERE I FILE SCARICATI
	//4)USCIRE

	draw_menu(&choice);
	//printf("Valore della scelta %d\n",choice);

	//switch a seconda della scelta operata dall'utente
	switch(choice){
		case LOGIN:
			//if(session_id.id == -1)
				login(sd);
			/*else{
				system("clear");
				printf("***************************************************************\n");
				printf("GIA' LOGGATO");
				printf("***************************************************************\n");					
				printf("Premi invio per tornare indietro ... \n");
				fflush(stdin);
				getchar();
			}*/ /*
			break;
		case CATALOGO:
			//Routine gestione catalogo e download
			catalogo(sd);
			break;
		case FILES_SCARICATI:
			scaricati();
		//	system("clear");
		//	execlp("ls","ls","-l","../FILES",NULL);
			break;
		//case FILES_SCARICANDO:
			//download();
			//break;
		case EXIT:
			/*Chiudo connessione*/ /*
			close_conn(sd);
			break;
		default: //impossibile entrare nel case default
			break;
	} */

		
	
	//DEBUG
	//eseguo un invio a una lettura.
	/* Inviamo dei dati */
	//	strcpy(buff,"Dati inviati dal client");
	//write_to_socket(sd,"Dati inviati dal client");

	/*riceviamo la risposta */
	//bzero(buff,MAX_MESS_SOCKET_LENGHT);
	//read_from_socket(sd, buff);

	//}

	return EXIT_SUCCESS;
}


