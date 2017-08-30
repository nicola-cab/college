#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compose.h"
#include "message.h"


/*
	Compone messaggio d'ok... tipicamente:
	0;<testo>;
*/
void compose_protocol_mess(int tipo, char * mess, char * tosend){

	//cancello i bytes associati alla stringa per evitare possibili problemi dovuti a messaggi inviati precedentemente
	bzero(tosend,strlen(tosend));
	//scrivo dentro il vettore tosend e compongo messaggio d'ok
	sprintf(tosend,"%d%s%s%s",tipo,sep,mess,sep);	
	//loggata
	compose_mess_to_log("Request da inviare al server: ",tosend,STRING_TYPE);
}

/*
	Concatena le stringhe passate all'interno del vettore di stringhe utilizzando il separatore ';'
*/
void cat(char * str[], char * sep, char * tosend, int num){

	//concateno i valori contenuti nel vettore di stringhe

	if(num!=0){

		strcpy(tosend,str[0]);
	
		int i=1;
		for(i;i<num;i++){
			strcat(tosend,sep);
			strcat(tosend,str[i]);
		}
	}
	
	return;
}

