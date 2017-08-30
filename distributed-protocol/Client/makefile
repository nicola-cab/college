#***************************************MACRO**********************************************
													
#definisco il path nel quale si trova la libreria sqlite3 (editare in base alle esigenze)
OBJ = /usr/include

#definisco opzioni per compilare il modulo per implementare la GUI
CFLAGS = `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0`

#******************************************************************************************


client: logger.o main.o socket.o message.o compose.o parse.o op.o close_manager.o db.o gui.o
	gcc -o client logger.o main.o socket.o message.o compose.o parse.o op.o close_manager.o gui.o db.o -Wall -W -O2 -Wl,-R$(OBJ) -lsqlite3 $(CFLAGS)
	echo "Compilazione eseguita con successo"

main.o: main.c 
	gcc -c -o main.o main.c $(CFLAGS)
	
socket.o: socket.c socket.h 
	gcc -c -o socket.o socket.c 

logger.o: logger.c logger.h
	gcc -c -o logger.o logger.c 

message.o: message.c message.h
	gcc -c message.c -o message.o

compose.o: compose.c compose.h
	gcc -c compose.c -o compose.o

parse.o: parse.c parse.h
	gcc -c parse.c -o parse.o

op.o:	op.c op.h
	gcc -c op.c -o op.o $(CFLAGS)

close_manager.o:	close_manager.c close_manager.h
	gcc -c close_manager.c -o close_manager.o

db.o: db.c db.h
	gcc -w -c db.c -o db.o -Wall -W -O2 -Wl,-R$(OBJ) -lsqlite3 $(CFLAGS)

gui.o: gui.c gui.h
	gcc -o gui.o -c gui.c $(CFLAGS) -lpthread
	
clear:
	rm -f client
	rm *.o
	rm *~
	echo "Pulizia binari del client eseguita"
