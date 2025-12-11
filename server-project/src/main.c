	/*
 * main.c
 *
 * UDP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a UDP server
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
#include <ctype.h> // necessario per tolower()


#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

#define NO_ERROR 0

#define INDIRIZZO_IP_SERVER "127.0.0.1"


#ifndef WIN32
char* strlwr(char *s) {
    for (char *p = s; *p; p++) {
        *p = tolower((unsigned char)*p);
    }
    return s;
}
#endif



float get_temperature(void) { // Range: -10.0 to 40.0 gradi C
    return (-10.0) + (float) rand() / RAND_MAX * ((40.0) - (-10.0));
}

float get_humidity(void) { // Range: 20.0 to 100.0 %
    return (20.0) + (float) rand() / RAND_MAX * ((100.0) - (20.0));
}

float get_wind(void) { // Range: 0.0 to 100.0 km/h
    return (float) rand() / RAND_MAX * (100.0);
}

float get_pressure(void) { // Range: 950.0 to 1050.0 hPa
    return (950.0) + (float) rand() / RAND_MAX * ((1050.0) - (950.0));
}

int findString(const char *target) {
    // 1. Controllo stringa nulla o vuota
    if (target == NULL || target[0] == '\0')
        return INVALID_REQ;

    // 2. Controllo lunghezza massima (protocollo: max 63 + terminatore)
    if (strlen(target) > 63)
        return INVALID_REQ;

    // 3. Controllo caratteri validi (solo lettere + spazi)
    for (int i = 0; target[i] != '\0'; i++) {
        if (!isalpha((unsigned char)target[i]) && target[i] != ' ')
            return INVALID_REQ;
    }

    for (int i = 0; target[i] != '\0'; i++) {
          if (isdigit((unsigned char)target[i]))
              return INVALID_CITY;
      }

    // Lista delle città valide
    const char *lista[] = {
        "ancona", "bari", "bologna", "cagliari", "catania",
        "firenze", "genova", "milano", "napoli", "palermo",
        "perugia", "pisa", "roma", "taranto",
        "torino", "trento", "trieste", "venezia", "verona"
    };

    int size = 20;

    // 4. Ricerca della città
    for (int i = 0; i < size; i++) {
        if (strcmp(lista[i], target) == 0)
            return VALID_REQ;   // trovato
    }

    return INVALID_CITY; // città valida come formato ma non esistente
}


void errorhandler(char *error_message) {
    printf("%s", error_message);
}

void clearwinsock() {
#ifdef WIN32
	WSACleanup();
#endif
}

void ErrorHandler(char *errorMessage) {
printf(errorMessage);
}




int main(int argc, char *argv[]) {

    int port = SERVER_PROTOPORT;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                port = atoi(argv[i + 1]);
                if (port <= 0) {
                    printf("Errore: porta non valida.\n\n");
                    printf("Uso corretto:\n  main.exe [-p porta]\n\n");
                    return 1;
                }
                i++;
            } else {
                printf("Errore: manca il numero di porta dopo -p.\n\n");
                printf("Uso corretto:\n  main.exe [-p porta]\n\n");
                return 1;
            }
        } else {
            printf("Errore: argomento sconosciuto: %s\n\n", argv[i]);
            printf("Uso corretto:\n  main.exe [-p porta]\n\n");
            return 1;
        }
    }

    printf("Porta selezionata: %d\n", port);


	// TODO: Implement server logic

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
	    struct sockaddr_in echoClntAddr;
	    unsigned int cliAddrLen;

	    // Buffer per ricevere i dati grezzi
	    char echoBuffer[ECHOMAX];
	    int recvMsgSize;

	    // TODO: Create UDP socket
	    if ((my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	        ErrorHandler("socket() failed");

	    // TODO: Configure server address
	    memset(&echoServAddr, 0, sizeof(echoServAddr));
	    echoServAddr.sin_family = AF_INET;
	    echoServAddr.sin_port = htons(port);
	    // Bind a INADDR_ANY permette di ricevere da qualsiasi interfaccia di rete (pratica standard)
	    // Se si vuole limitare solo a localhost usa: inet_addr(INDIRIZZO_IP_SERVER);
	    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	    // TODO: Bind socket
	    if ((bind(my_socket, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))) < 0)
	        ErrorHandler("bind() failed");

	    printf("In attesa di richieste...\n");

	    // Strutture per la logica meteo

	    weather_response_t resp;
	    memset(&resp, 0, sizeof(resp));


	    // TODO: Implement UDP datagram reception loop
	    while (1) {
	        cliAddrLen = sizeof(echoClntAddr);

	        // 1. RICEZIONE DATI
	        recvMsgSize = recvfrom(my_socket, echoBuffer, ECHOMAX, 0,
	                (struct sockaddr *) &echoClntAddr, &cliAddrLen);

	        if(recvMsgSize < 0){
	            ErrorHandler("recvfrom() failed");
	            continue; // Non uscire dal loop, aspetta il prossimo pacchetto
	        }

	        // Controllo dimensione minima
	        if (recvMsgSize != sizeof(weather_request_t)) {
	            printf("Ricevuto pacchetto di dimensione errata (%d byte). Ignorato.\n", recvMsgSize);
	            continue;
	        }

	        weather_request_t req;
	        int offset = 0;

	        // type (1 byte)
	        memcpy(&req.type, echoBuffer + offset, sizeof(char));
	        offset += sizeof(char);

	        // city (64 byte)
	        memcpy(req.city, echoBuffer + offset, sizeof(req.city));
	        offset += sizeof(req.city);

	        // garantire null-termination
	        req.city[63] = '\0';

	        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

	        // 2. ELABORAZIONE LOGICA METEO

	        // Normalizzazione input
	        strlwr(req.city);
	        req.type = tolower((unsigned char)req.type);

	        printf("Richiesta -> Citta: %s, Tipo: %c\n", req.city, req.type);

	        // Verifica Citta'
	        // Assicurati che questi valori corrispondano al protocollo (0, 1, 2)
	        // Qui assumo: 0 = OK, 1 = Citt� Errata, 2 = Richiesta Errata
	        resp.status = findString(req.city);

	        if (resp.status == VALID_REQ) {
	                 switch (req.type) {
	                 case 't':
	                     resp.value = get_temperature();
	                     resp.type = req.type;
	                     break;
	                 case 'p':
	                     resp.value = get_pressure();
	                     resp.type = req.type;
	                     break;
	                 case 'h':
	                     resp.value = get_humidity();
	                     resp.type = req.type;
	                     break;
	                 case 'w':
	                     resp.value = get_wind();
	                     resp.type = req.type;
	                     break;
	                 default:
	                     resp.status = INVALID_REQ;
	                     break;
	                 };
	             } else {
	                 resp.value = 0.0;
	             };

	        // Output colorato lato server per debug
	        if(resp.status == VALID_REQ)      printf("\033[32mStatus: OK (Invio %.2f)\033[0m\n", resp.value);
	        else if(resp.status == INVALID_CITY) printf("\033[31mStatus: Citt� non valida\033[0m\n");
	        else                      printf("\033[31mStatus: Tipo non valido\033[0m\n");
	        printf("------------------------------------------------\n");

	        // 3. INVIO RISPOSTA
	        char buffer[sizeof(uint32_t) + sizeof(char) + sizeof(float)];
	        offset = 0;

	        // Serializza status (con network byte order)
	        uint32_t net_status = htonl(resp.status);
	        memcpy(buffer + offset, &net_status, sizeof(uint32_t));
	        offset += sizeof(uint32_t);

	        // Serializza type (1 byte, no conversione)
	        memcpy(buffer + offset, &resp.type, sizeof(char));
	        offset += sizeof(char);

	        // Serializza value (float con network byte order)
	        uint32_t temp;
	        memcpy(&temp, &resp.value, sizeof(float));
	        temp = htonl(temp);
	        memcpy(buffer + offset, &temp, sizeof(float));
	        offset += sizeof(float);
	        ssize_t sent =sendto(my_socket,buffer, offset, 0,
	                (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr));

			if (sent != offset) {
				ErrorHandler("sendto() sent different number of bytes than expected");
			}
	    }

	    printf("Server terminated.\n");

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end
