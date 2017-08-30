#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compose.h"
#include "message.h"

//massimo numero di contenuti che posso leggere dal server (da migliorare la gestione)
#define CUSTOM_NUM 100

/*
	Parser per eleaborare il messaggio di risposta del server,
	restituisce il numero di stringhe splittate e modifica il 
	vettore di stringhe passato come parametro
*/

int parse(char mess[], char ** response, int * ret){

	compose_mess_to_log("Inizio ad eseguire il parsing per la request: ",mess,STRING_TYPE);

	//int ret = 0;

	//alloco il vettore solo se è non punta a nessun altra area di memoria
	if(response == NULL)
		response = (char**)malloc(CUSTOM_NUM * sizeof(char*));	

	*ret = split_mess(mess,response);

	int i = 0;
	for(i;i<*ret;i++)
		compose_mess_to_log("Valore splittato: ",response[i],STRING_TYPE);		

	//debug
	//if(response != NULL)
	//	logger("Vettore NOT NULL dopo la creazione in fase di splitting");

	//logica per la return, Parser con Zero stringhe trovate = ERRORE
	if(*ret == 0)
		return ERROR;
	else
		return OK;
}

/*
	Splitto la stringa utilizzando il separatore ';'
	Restituisce il numero di stringhe ottenute dallo split.
*/
int split_mess(char mess[] ,char ** values){

	logger("Eseguo split della request");

	int i=0,j=0,start=0;
	int len = strlen(mess)+1;
	int flag = 0;
	int num_stringhe=0;

	char buff[50];
	
	//logger("Inzio do while");
	//printf("%d\n",len);

	//esporto la prima porzione del messaggio
	do{
		//setto il flag a uno per ogni separatore che trovo
		//logger("Eseguo confronto");
		if(mess[i] == sep[0]){
			++flag;
			num_stringhe ++;
		}
		else
			//setto a zero il flag per ogni carattere da copiare (non trovo nessun separatore)
			flag = 0;

		//copio il carattere in un buffer
		if(flag == 0){
			//logger("Copio carattere per carattere");
			buff[j++] = mess[i];
		}

		//copio nel vettore di stringhe 
		else{
			//logger("Aggiungo stringa al vettore");
			buff[j] = '\0'; //set del terminatore 
			//printf("%d\n",flag);
			//compose_mess_to_log("num: ",&num_stringhe,INTEGER_TYPE);
			values[num_stringhe-1] = (char *)malloc(strlen(mess)+1); //alloco la memoria
			strcpy(values[num_stringhe-1],buff);	//copio la stringa
			j=0;	//reset dell'indice che utilizzo per riempire il buffer temporaneo
			//flag = 0;
		}
		//mi porto avanti di un carattere
		i++;		
	}while(i<len);

	compose_mess_to_log("Numero di stringhe ottenute dallo splitting: ",&num_stringhe,INTEGER_TYPE);

	return num_stringhe;
}
