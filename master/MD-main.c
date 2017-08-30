/*
 * main.c
 *
 *  Created on: Jan 26, 2012
 *      Author: nicola
 */


#include "MD.h"
#include "signal.h"


inline void pabort(string msg)
{
	printf("%s\n",msg);
	exit(-1);
}

void handler_sigint(void)
{

	printf("Handler\n");

	//Close server
	bool status = FALSE;
	status = MD_down_Server();
	if(FALSE == status)
	{
		pabort("MD closed uncorrectly\n");
	}
	else
		printf("MD closed correctly\n");

	exit(0);
}

void handler_sigkill(void)
{
	bool status = FALSE;
	status = MD_down_Server();
	if(FALSE == status)
	{
		pabort("MD closed uncorrectly\n");
	}
	else
		printf("MD closed correctly\n");

	exit(0);
}

inline void MD_Server_routine()
{
	//here business testing MD wrapper
	bool status;

	//handler to manage CTRL+C keyboard command
	signal(SIGINT, handler_sigint);
	//handler to manage kill command
	signal(SIGKILL, handler_sigkill);

	status = MD_up_Server();

	if(FALSE == status)
	{
		printf("Error to launch server");
	}
}

int main(int argc, char**argv)
{

	if(argc < 4 ){
		printf("Error ... you must pass input file to orac [Usage ./<orac_executable> <id> <input_file> <network_interface> <path_where_store_discovery_information_file>  > output_file] \n");
		return -1;
	}

	char errorMsg[80];
	int my_id;

	//first parameter is MD

	//MD id
	int id_MD = atoi(argv[1]);

	//Input file where read molecular dynamic info
	char * input_file = argv[2];

	//Network interface (e.g lo,eth0, etc.)
	char * net_interface = argv[3];

	//Path where save discovery informations
	char * discover_info = argv[4];

	printf("Passed parameters \n");
	printf("ID MD : %d \n", id_MD);
	printf("Input file used for molecular dynamic simulation : %s\n", input_file);
	printf("Network interface where MD server is available --> %s\n", net_interface);
	string md_discover_path = (string)malloc(100);
	sprintf(md_discover_path,"%s%s", discover_info, SHARED_MEMORY_DISCOVERY_FILE);

	set_Replica_ID(id_MD);  //Setting Replica id file name
	set_MD_Network_interface(net_interface); //Setting network interface where MD will be discoverable
	set_path_discovery_file(md_discover_path);  //Setting discovery file path
	free(md_discover_path);

	//only to test
	orac_md_init_(input_file, strlen(input_file), errorMsg, strlen(errorMsg), &my_id);

	//Bring up MD Server
	MD_Server_routine();

	return 0;
}
