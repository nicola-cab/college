/*
FUNZIONI PER LOGGARE. UTILE PER IL TRACE DEL SERVER
*/

#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_MESS_LENGHT 150
#define MAX_LOG_LENGHT 200

//Stringa per definire path relativo per specificare il percorso del file di log
char * PATH_LOG = "../LOG/client.log"; 
//Vettore di stringhe per definire i mesi dell'anno
char *mesi[] = {"Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio", "Giugno","Luglio", "Agosto", "Settembre", "Ottobre", "Novembre", "Dicembre"};
//Vettore di stringhe per definire i giorni della settimana
char *giorni[] = {"Domenica", "Lunedì", "Martedì", "Mercoledì", "Giovedì", "Venerdì", "Sabato"};

/*inserisce loggata per il server*/
void logger(char * str);

/*Preleva ora e data del sistema*/
void get_local_hour();
