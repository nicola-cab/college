/*
 * MD_discover.c
 *
 *  Created on: 04/mag/2012
 *      Author: nicola
 */

#ifdef MD_REPLICA__

#include "MD_discover.h"

extern int MD_sock;

extern STATUS MD_task_status;

#define INTERFACE "lo"

//Network interface
char g_network_interface[50] = INTERFACE;

bool Discover_MD_IP(md_discovery_t * md_entry)
{
	debug_MD_log("Discovery of new MD");

	bool status = FALSE;
	string * ip;

	if( (MD_DOWN == MD_task_status))
	{
		//ok.. retrieve ip address

		int domains[] = { AF_INET , AF_INET6 }; //Ipv6 for the future
		ip = getIpAddress(domains[0]);

		if(ip == NULL)
		{
			critical_MD_log("Error to retrieve ip address for MD");
		}
		else
		{
			info_MD_log("MD Unit discovered correctly ");

			debug_MD_log(ip[0]);
			debug_MD_log(ip[1]);

			md_entry->MD_interface = (string)malloc(strlen(ip[0]));
			md_entry->MD_ip_address = (string)malloc(30);//strlen(ip[1]));

			//ok prepare new entry
			strcpy(md_entry->MD_interface, ip[0]);
			strcpy(md_entry->MD_ip_address,ip[1]);
			md_entry->MD_status = MD_UP;  //now MD is down ... after this value will be changed in the code
			md_entry->next = NULL;

			//free(ip[0]);
			//free(ip[1]);
			free(ip);

			debug_MD_log("FINE");

			status = TRUE;

		}
	}
	return status;
}

bool save_on_file(md_discovery_t * entry)
{
	bool status = FALSE;
	ssize_t nbyes = 0;
	char entry_str[80];
	memset(entry_str,0,80);
	FILE *fp = NULL;


	if( MD_UP == MD_task_status ) //ok MD is up...save on file
	{
		fp = fopen(get_path_discovery_file(), "a");
		if(!fp)
		{
			critical_MD_log("Unable to create discovery file .. please check path ");
			critical_MD_log(get_path_discovery_file());
		}
		else
		{
			sprintf(entry_str, "%s-%s-%d\n", entry->MD_interface, entry->MD_ip_address,entry->MD_status);
			nbyes = fprintf(fp, "%s", entry_str);

			if(nbyes < 0)
			{
				//big error ... MD never been discovered!
				error_MD_log("Error to write on shared memory file !!! big Error, MD NEVER BEEN DISCOVERED");
				status = ERROR;
			}
			else
			{
				status = TRUE;
			}
		}
	}

	fclose(fp);
	return status;

}


string* getIpAddress(const int domain)
{
	debug_MD_log("Get ip of MD Unit");

	string * ip_r = (string*)malloc(sizeof(string)*2);
	ip_r[0] = (string)malloc(100);
	ip_r[1] = (string)malloc(30);

	string msg_log[LOG_STR_LEN]; // = (string)malloc(sizeof(char)*LOG_STR_LEN);
	memset(msg_log,0,LOG_STR_LEN);

	struct ifconf ifconf;
	struct ifreq ifr[50];
	int ifs;
	int i;
	bool status = FALSE;

	if(MD_DOWN == MD_task_status)
	{

		ifconf.ifc_buf = (char *) ifr;
		ifconf.ifc_len = sizeof ifr;

		if (ioctl(MD_sock, SIOCGIFCONF, &ifconf) == -1) {
			critical_MD_log("Error to call ioctl to retrieve ip ");
		}

		ifs = ifconf.ifc_len / sizeof(ifr[0]);
		sprintf(msg_log,"interfaces = %d:",ifs);
		info_MD_log(msg_log);

		for (i = 0; i < ifs; i++) {
			char ip[INET_ADDRSTRLEN];
			struct sockaddr_in *s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;
			if (!inet_ntop(domain, &s_in->sin_addr, ip, sizeof(ip))) {
				critical_MD_log("inet_ntop fail");
			}

			//g_netwrok_interface is passed from routine which invokes the MD Server startup
			//Default: interface is loopback
			if( !strcmp(ifr[i].ifr_name,g_network_interface) )
			{
				strcpy(ip_r[0], ifr[i].ifr_name);
				strcpy(ip_r[1], ip);

				debug_MD_log("Network interface and ip address for MD");
				debug_MD_log(ip_r[0]);
				debug_MD_log(ip_r[1]);

				status = TRUE;
			}
		}
	}

	return status ? ip_r : NULL;
}

#endif
