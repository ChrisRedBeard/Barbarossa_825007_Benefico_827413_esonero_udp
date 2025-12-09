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

	        if (sscanf(argv[i+1], "%c %64[^\n]", &req.type, req.city) != 2) {
	            fprintf(stderr, "Errore: formato richiesto deve essere \"type city\"\n");
	            return -1;
	        }
	        size_t city_len = strlen(req.city);
	        if (city_len > 63) {
	            fprintf(stderr, "Errore: il nome della citta' e' troppo lungo (%zu caratteri, massimo 63).\n", city_len);
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

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	//server ip è il mio nome simbolico che vado ad interpretare come indirizzo ip
	struct sockaddr_in echoServAddr;
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_port = htons(port);

	struct hostent *host = NULL;
	struct in_addr addr;

	// Proviamo a interpretare server_ip come IP
	#if defined(WIN32)
	addr.s_addr = inet_addr(server_ip);  // restituisce INADDR_NONE se fallisce
	if (addr.s_addr != INADDR_NONE) {
	    // Reverse lookup su Windows
	    host = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
	    if (host != NULL) {
	        snprintf(server_ip, sizeof(server_ip), "%s", host->h_name);
	    }
	    memcpy(&echoServAddr.sin_addr, &addr, sizeof(addr));
	} else {
	    // Forward lookup
	    host = gethostbyname(server_ip);
	    if (host == NULL) {
	        ErrorHandler("Errore nel risolvere l'host");
	        clearwinsock();
	        return EXIT_FAILURE;
	    }
	    memcpy(&echoServAddr.sin_addr, host->h_addr_list[0], host->h_length);
	    snprintf(server_ip, sizeof(server_ip), "%s", host->h_name);
	}
	#else
	// Linux/macOS
	if (inet_pton(AF_INET, server_ip, &addr) == 1) {
	    host = gethostbyaddr(&addr, sizeof(addr), AF_INET);
	    if (host != NULL) {
	        snprintf(server_ip, sizeof(server_ip), "%s", host->h_name);
	    }
	    memcpy(&echoServAddr.sin_addr, &addr, sizeof(addr));
	} else {
	    host = gethostbyname(server_ip);
	    if (host == NULL) {
	        ErrorHandler("Errore nel risolvere l'host");
	        clearwinsock();
	        return EXIT_FAILURE;
	    }
	    memcpy(&echoServAddr.sin_addr, host->h_addr_list[0], host->h_length);
	    snprintf(server_ip, sizeof(server_ip), "%s", host->h_name);
	}
	#endif


	// TODO: Implement client logic

	int my_socket;

	    struct sockaddr_in fromAddr;


	    // TODO: Creazione Socket
	    if ((my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	        ErrorHandler("socket() failed");




	    // TODO: Logica di comunicazione UDP

	    // Costruzione buffer richiesta
	        char sendBuffer[sizeof(uint32_t) + sizeof(char) + sizeof(float)];
	        int offset = 0;


	        // Serializza type
	        memcpy(sendBuffer + offset, &req.type, sizeof(char));
	        offset += sizeof(char);

	        // Serializza city (64 byte fissi)
	        memcpy(sendBuffer + offset, req.city, sizeof(req.city));
	        offset += sizeof(req.city);



	        // Invio richiesta
	        if (sendto(my_socket, sendBuffer, sizeof(weather_request_t), 0,
	                   (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) != sizeof(weather_request_t)) {
	            ErrorHandler("sendto() failed");
	            closesocket(my_socket);
	            clearwinsock();
	            return -1;
	        }

	        // Ricezione risposta
	        unsigned int fromLen = sizeof(fromAddr);
	        char recvBuffer[sizeof(uint32_t) + sizeof(char) + sizeof(float)];
	        int recvLen = recvfrom(my_socket, recvBuffer, sizeof(recvBuffer), 0,
	                               (struct sockaddr*)&fromAddr, &fromLen);
	        if (recvLen != (sizeof(uint32_t) + sizeof(char) + sizeof(float))) {
	            ErrorHandler("pacchetto di risposta di dimensione errata");
	            closesocket(my_socket);
	            clearwinsock();
	            return -1;
	        }
	         // Interpretazione risposta : deserializzazione
	        weather_response_t resp;
	        offset = 0;

	        // 1. Deserializza status
	        uint32_t net_status;
	        memcpy(&net_status, recvBuffer + offset, sizeof(uint32_t));
	        resp.status = ntohl(net_status);
	        offset += sizeof(uint32_t);

	        // 2. Deserializza type (1 byte)
	        memcpy(&resp.type, recvBuffer + offset, sizeof(char));
	        offset += sizeof(char);

	        // 3. Deserializza value (float)
	        uint32_t temp;
	        memcpy(&temp, recvBuffer + offset, sizeof(uint32_t));
	        temp = ntohl(temp);   // riconverti in host byte order
	        memcpy(&resp.value, &temp, sizeof(uint32_t)); // reinterpretazione come float
	        offset += sizeof(uint32_t);


	        switch(resp.status){
	            case 1:
	                printf("Ricevuto risultato dal server %s (ip %s). Citta' non disponibile\n",server_ip,inet_ntoa(*(struct in_addr *)host->h_addr));
	                break;
	            case 2:
	                printf("Ricevuto risultato dal server %s (ip %s). Richiesta non valida\n",server_ip,inet_ntoa(*(struct in_addr *)host->h_addr));
	                break;
	            default:
	                printf("Ricevuto risultato dal server %s (ip %s). %s: %s\n",server_ip,inet_ntoa(*(struct in_addr *)host->h_addr),req.city, (valueToString(resp.type, resp.value)) );
	                break;
	        }


	// TODO: Close socket
	closesocket(my_socket);
	printf("Client terminated.\n");
	clearwinsock();
	return 0;

} // main end
