#include "logger.h"

/****************************************************************************
	APRE FILE IN APPEND ED INSERISCE MESSAGGIO DI LOG PASSATO COME PARAMETRO
****************************************************************************/

void logger(char * mess){

	FILE * fp;
	fp = fopen(PATH_LOG,"a");

	if(!fp)
		printf("Errore grave, impossibile eseguire una loggata per il server");

	else{
		char loggata [MAX_LOG_LENGHT];
		get_local_hour(loggata);
		//concateno le stringhe
		strcat(loggata,mess);
		//mando a capo per le prossime loggate
		strcat(loggata,"\n");
		//scrivo su file
		int ret = fputs(loggata,fp);
		//controllo che la scrittura non sia fallita 
		if(ret == EOF)
			printf("Scrittura fallita sul file server.log");		
	}
	fclose(fp);
	return;
}


/***************************************************************************
LEGGE ORA & DATA DEL SISTEMA
***************************************************************************/

void get_local_hour(char timestamp[]){

	bzero(timestamp,strlen(timestamp)+1);

	//Strutture dati per gestire i tipi di dati per le date e il tempo tempo
	time_t rawtime;
	time (&rawtime);
	struct tm* leggibile;

	//costruisco la stringa 
	char open  = '[';
	char close = ']';
	char sep   = ':';
	char sep_1 = '-';	
	char space = ' '; 

	//Leggo localtime	
	leggibile = localtime(&rawtime);
   
	//calcola l'anno
	int year = leggibile->tm_year + 1900;
	//mese dell'anno
	char * mese = mesi[leggibile->tm_mon];
	//giorno in formato numerico
	int day = (leggibile->tm_mday);
	//ora
	int hour = leggibile->tm_hour;
	//minuti
	int minute = leggibile->tm_min;
	//secondi
	int sec = leggibile->tm_sec;
	//giorno della settimana (unused)
	char * giorno_sett = giorni[leggibile->tm_wday];
	//costuisco stringa 
	sprintf(timestamp,"%c%d%c%s%c%d%c%d%c%d%c%d%c%c",open,year,sep_1,mese,sep_1,day,space,hour,sep,minute,sep,sec,close,space); 
	
	//debug
	//printf("Valore del timestamp corrente: %s\n",timestamp);
}
