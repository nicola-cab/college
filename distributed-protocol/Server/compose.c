#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compose.h"
#include "message.h"

//separatori utilizzati per concatenare le stringhe
char sep[1] = {';'};


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
	compose_mess_to_log("Response da inviare al client: ",tosend,STRING_TYPE);
}

/*
	Concatena le stringhe passate all'interno del vettore di stringhe utilizzando il separatore ';'
*/
void cat(char str[][N], char * sep, char * tosend, int num){

	//concateno i valori contenuti nel vettore di stringhe

	if(num!=0){
	
		char tmp[20];
		sscanf(str[0],"%s",tmp);
		strcpy(tosend,tmp);
	
		int i=1;
		for(i;i<num;i++){
			bzero(tmp,20);
			sscanf(str[i],"%s",tmp);
			strcat(tosend,sep);
			strcat(tosend,tmp);
		}
	}
	
	return;
}

