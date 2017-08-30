/*
	Dichiaro:
	terminatore per capire quale sia l'ultimo carattere da leggere sul socket (non dovrei averne bisogno se leggo sempre 1000 caratteri, svuoto sempre il socket)
	sep per separare le varie componenti del messaggio
*/

//char terminatore = '\n';
extern char sep[1];


/*
	Definisce le costanti e le funzioni che permettono al client di comporre i messaggi da 
	spedire al server
*/

#define OK 1
#define ERROR -1
#define REQUEST_LOGIN 100
#define REQUEST_CATALOGO 200
#define REQUEST_CATALOGO_ITEMS 201
#define REQUEST_FILE 300
#define REQUEST_CHANNEL 400
#define REQUEST_CLOSE 500

/*
	Routine per comporre un nuovo messaggio da spedire verso il server
*/

void compose_protocol_mess(int tipo, char * mess, char * tosend);

/*	
	Sub Routine utilizzate per esigenze specifiche, come ad esempio
*/

/*
	Dato il vettore di stringhe passato come parametro, la routine concatena le stringhe
	aggiungendo fra di esse il separatore passato come parametro
*/

void cat(char ** str, char * sep, char * tosend, int num);

