/*
 * main.c
 *
 * UDP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a UDP client
 * portable across Windows, Linux, and macOS.
 */

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

#define NO_ERROR 0
#define INDIRIZZO_IP_SERVER "127.0.0.1"

void ErrorHandler(char *errorMessage) {
printf(errorMessage);
}

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

char* valueToString(char tipo,float value){
    static char temp[40];
    switch (tipo) {
            case 't':
                 snprintf(temp,sizeof(temp),"Temperatura = %.1f \u00B0C",value);
                break;
            case 'p':
                snprintf(temp,sizeof(temp),"Pressione = %.1f hPA",value);
                break;
            case 'h':
                snprintf(temp,sizeof(temp),"Umidita' = %.1f %%",value);
                break;
            case 'w':
                snprintf(temp,sizeof(temp),"Vento = %.1f Km/h",value);
                break;
    }
    return temp;
}

int main(int argc, char *argv[]) {

	// ---------------- PARSING ARGOMENTI ----------------
	char server_ip[64] = INDIRIZZO_IP_SERVER;   // default
	int port = SERVER_PROTOPORT;                // default
	weather_request_t req;

	if (argc < 3) {
	    fprintf(stderr, "Uso: %s [-s server] [-p port] -r \"type city\"\n", argv[0]);
	    return -1;
	}

	int i = 1;
	int found_r = 0;

	while (i < argc) {

	    // -s server
	    if (strcmp(argv[i], "-s") == 0) {
	        if (i + 1 >= argc) {
	            fprintf(stderr, "Errore: manca il valore dopo -s\n");
	            return -1;
	        }
	        snprintf(server_ip, sizeof(server_ip), "%s", argv[i+1]);
	        i += 2;
	        continue;
	    }

	    // -p port
	    if (strcmp(argv[i], "-p") == 0) {
	        if (i + 1 >= argc) {
	            fprintf(stderr, "Errore: manca la porta dopo -p\n");
	            return -1;
	        }
	        port = atoi(argv[i+1]);
	        i += 2;
	        continue;
	    }

	    // -r "type city"
	    if (strcmp(argv[i], "-r") == 0) {
	        if (i + 1 >= argc) {
	            fprintf(stderr, "Errore: manca la stringa dopo -r\n");
	            return -1;
	        }

	        if (sscanf(argv[i+1], "%c %39[^\n]", &req.type, req.city) != 2) {
	            fprintf(stderr, "Errore: formato richiesto deve essere \"type city\"\n");
	            return -1;
	        }

	        found_r = 1;
	        i += 2;
	        continue;
	    }

	    fprintf(stderr, "Parametro sconosciuto: %s\n", argv[i]);
	    return -1;
	}

	if (!found_r) {
	    fprintf(stderr, "Errore: il parametro -r e' obbligatorio\n");
	    return -1;
	}


	// TODO: Implement client logic

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	int my_socket;

	struct sockaddr_in echoServAddr;
	struct sockaddr_in fromAddr;
	typedef int socklen_t;
	socklen_t fromSize;

	// TODO: Create UDP socket

	if ((my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	ErrorHandler("socket() failed");


	// TODO: Configure server address

	memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = PF_INET;
		echoServAddr.sin_port = htons(port);
		echoServAddr.sin_addr.s_addr = inet_addr(server_ip);

	// TODO: Implement UDP communication logic

	weather_response_t resp;

	if (sendto(my_socket, (const char*)&req, sizeof(weather_request_t), 0, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) != sizeof(weather_request_t))
	ErrorHandler("sendto() sent different number of bytes than expected");

	fromSize = sizeof(fromAddr);
	if(recvfrom(my_socket,(char*) &resp, sizeof(weather_response_t), 0, (struct sockaddr*)&fromAddr, &fromSize) != sizeof(weather_response_t))
		ErrorHandler("recvfrom() Error");

	if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
	{
	fprintf(stderr, "Errore: ricevuto un pacchetto da una fonte sconosciuta.\n");
	exit(EXIT_FAILURE);
	}

	switch(resp.status){
		  case 1: printf("Ricevuto risultato dal server ip %s Citta' non disponibile",INDIRIZZO_IP_SERVER);
		  	  	  break;
		  case 2: printf("Ricevuto risultato dal server ip %s Richiesta non valida",INDIRIZZO_IP_SERVER);
		  	  	  break;
		  default:	printf("Ricevuto risultato dal server ip %s %s: %s",server_ip,req.city,valueToString(resp.type,resp.value));
		  	  	  break;
		 }




	// TODO: Close socket
	closesocket(my_socket);
	printf("Client terminated.\n");
	clearwinsock();
	return 0;
} // main end
