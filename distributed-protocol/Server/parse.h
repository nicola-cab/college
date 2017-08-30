/*	
	Definisce le routine che permettono di eseguire il parsing del messaggio 
	inviato dal client.

*/

/*
Effettua il parsing del messaggio a seconda del tipo della risposta che si intende parsare. ES:
tipo: LOGIN, ci si aspetta di trovarsi difronte a solo tre situazioni:

	-1;<testo>;
	0;<testo>;
	400;<porta> (richiesta apertura nuovo canale socket)

Tutto il resto viene scartato e trattato come se fosse un errore

*/
int parse(char * mess, char ** response, int * ret);

/*
	Permette di splittare il messaggio inviato, ricercando il sepatatore ;
*/
int split_mess(char *,char **);

