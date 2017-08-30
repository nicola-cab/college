# compilazione dell'applicazione lato utente
DEPENDENT_FILES_MONITOR = monitor.c monitor.h time_monitor.h server.c server.h \
                          menu.c menu.h tx_rx.c tx_rx.h Makefile

DEPENDENT_FILES_CLIENT = client.c tx_rx.c tx_rx.h Makefile

DEPENDENT_FILES_MODULE = time_monitor.c time_monitor.h Makefile


obj-m	:= time_monitor.o


monitor: $(DEPENDENT_FILES_MONITOR)
	gcc -o monitor monitor.c server.c menu.c tx_rx.c -lpthread
	chmod +x monitor
	

client: $(DEPENDENT_FILES_CLIENT)
	gcc -o client client.c tx_rx.c -lpthread
	chmod +x client
	
	
clean:
	rm -f *.o *.ko Module.symvers *.mod.c .*.cmd monitor client
	rm -rf .tmp_versions		
	