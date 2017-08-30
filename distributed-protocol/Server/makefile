#***************************************MACRO**********************************************
													
#definisco il path nel quale si trova la libreria sqlite3 (editare in base alle esigenze)
OBJ = /usr/include

#******************************************************************************************

#compilazione di tutti i sorgenti e linking degli stessi (genero eseguibile server)
server: main.o socket.o logger.o close_manager.o message.o compose.o parse.o db.o op.o 
	gcc -o server main.o socket.o logger.o close_manager.o message.o compose.o parse.o op.o db.o -Wall -W -O2 -Wl,-R$(OBJ) -lsqlite3
	echo "Compilazione eseguita con successo"

#compila il main
main.o: main.c
	gcc -c -o main.o main.c 

#compila il codice per la gestione dei socket	
socket.o: socket.c socket.h
	gcc -c -o socket.o socket.c 

#compila il logger, ossia il codice che scrive su file tutti gli eventi del server
logger.o: logger.c logger.h
	gcc -c -o logger.o logger.c 

#compila il gestore dei processi zombie e gestore dei canali socket da chiudere
close_manager.o: close_manager.c close_manager.h
	gcc -c -o close_manager.o close_manager.c 

#compilo routine per la gestione dei messaggi del protocollo
message.o: message.c message.h
	gcc -c message.c -o message.o

#compilo routine per comporre messaggi seguendo il protocollo
compose.o: compose.c compose.h
	gcc -c compose.c -o compose.o

#compilo routine per eseguire il parse dei messaggi
parse.o: parse.c parse.h
	gcc -c parse.c -o parse.o

#compila routine per la gestione delle operazioni definite nel protocollo (es LOGIN,etc)
op.o:	op.c op.h
	gcc -c op.c -o op.o

#compilo sorgenti per accesso al database (attenzione necessita della libreria sqlite3 per C)
db.o: db.c db.h
	gcc -w -c db.c -o db.o -Wall -W -O2 -Wl,-R$(OBJ) -lsqlite3

#pulizia dei sorgenti compilati	
clear:
	rm -f server
	rm -f *.o
	rm -f *~
	echo "Pulizia binari del server eseguita"
