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

#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

#define NO_ERROR 0


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
    char *lista[] = { "ancona", "bari", "bologna", "cagliari", "catania",
            "firenze", "genova", "milano", "napoli", "palermo", "perugia",
            "pisa", "reggio calabria", "roma", "taranto", "torino", "trento",
            "trieste", "venezia", "verona" };
    int size = 20;
    for (int i = 0; i < size; i++) {
        if (strcmp(lista[i], target) == 0)
            return VALID_REQ; // trovato
    }
    return INVALID_CITY; // non trovato
}

void errorhandler(char *error_message) {
    printf("%s", error_message);
}

void handleClientConnection(int client_socket);

void handleClientConnection(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received;
    weather_response_t response;
    weather_request_t request;

    printf("Client connesso! In attesa di messaggi...\n");

    while (1) {

        bytes_received = recv(client_socket, (char*) &request,
                sizeof(weather_request_t), 0);

        // Conversione in minuscolo PORTABILE
        strlwr(request.city);
        request.type = tolower((unsigned char)request.type);

        response.status = findString(request.city);

        if (response.status == VALID_REQ) {
            switch (request.type) {
            case 't':
                response.value = get_temperature();
                response.type = request.type;
                break;
            case 'p':
                response.value = get_pressure();
                response.type = request.type;
                break;
            case 'h':
                response.value = get_humidity();
                response.type = request.type;
                break;
            case 'w':
                response.value = get_wind();
                response.type = request.type;
                break;
            default:
                response.status = INVALID_REQ;
                break;
            };
        } else {
            response.value = 0.0;
        };

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';

            printf("Tipo richiesto ricevuta: %c\n", request.type);
            printf("Città richiesta ricevuta: %s\n\n", request.city);
            printf("INVIO RISPOSTA IN CORSO...\n");

            if(response.status==VALID_REQ){
                printf("\033[32mStatus:0\033[0m\n");
            }else if(response.status==INVALID_REQ){
                printf("\033[31mStatus:2\033[0m\n");
            }else if(response.status==INVALID_CITY){
                printf("\033[31mStatus:1\033[0m\n");
            };
            printf("invio risposta:Tipo: %c\n", response.type);
            printf("invio risposta:Valore: %.2f\n", response.value);

            if (send(client_socket, (char*) &response,
                    sizeof(weather_response_t), 0)
                    != sizeof(weather_response_t)) {
                errorhandler(
                        "send() sent a different number of bytes than expected");
                closesocket(client_socket);
                clearwinsock();
            }

            if (strcmp(buffer, "quit") == 0) {
                printf("Client ha richiesto la disconnessione.\n");
                break;
            }
        } else if (bytes_received == 0) {
            printf("Client disconnesso.\n");
            break;
        } else {
            errorhandler("recv() failed.\n");
            break;
        }
    }

    closesocket(client_socket);
}




void ErrorHandler(char *errorMessage) {
printf(errorMessage);
}

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}


int main(int argc, char *argv[]) {

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
	char echoBuffer[ECHOMAX];
	int recvMsgSize;
	// TODO: Create UDP socket

	if ((my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	ErrorHandler("socket() failed");

	// TODO: Configure server address

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_port = htons(PORT);
	echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	// TODO: Bind socket
	if ((bind(my_socket, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))) < 0)
	ErrorHandler("bind() failed");

	// TODO: Implement UDP datagram reception loop 
	while (1) {
		cliAddrLen = sizeof(echoClntAddr);
		recvMsgSize = recvfrom(my_socket, echoBuffer, ECHOMAX, 0,
				(struct sockaddr *) &echoClntAddr, &cliAddrLen);
		printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
		printf("Received: %s\n", echoBuffer);
		// RINVIA LA STRINGA ECHO AL CLIENT
		if (sendto(my_socket, echoBuffer, recvMsgSize, 0,
				(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr))
				!= recvMsgSize)
			ErrorHandler("sendto() sent different number of bytes than expected");
	}

	printf("Server terminated.\n");

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end
