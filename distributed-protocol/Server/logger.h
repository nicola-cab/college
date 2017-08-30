/*
FUNZIONI PER LOGGARE. UTILE PER IL TRACE DEL SERVER
*/

#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_MESS_LENGHT 150
#define MAX_LOG_LENGHT 200

//Stringa per definire path relativo per specificare il percorso del file di log
extern char * PATH_LOG;
//Vettore di stringhe per definire i mesi dell'anno
extern char *mesi[];
//Vettore di stringhe per definire i giorni della settimana
extern char *giorni[];

/*inserisce loggata per il server*/
void logger(char * str);

/*Preleva ora e data del sistema*/
void get_local_hour();
