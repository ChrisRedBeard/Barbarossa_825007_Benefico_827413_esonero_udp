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

void ErrorHandler(char *errorMessage) {
printf(errorMessage);
}

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

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
	char echoString[ECHOMAX];
	char echoBuffer[ECHOMAX];
	int echoStringLen;
	int respStringLen;
	printf("Inserisci la stringa echo da inviare al server\n");
	scanf("%s", echoString);
	if ((echoStringLen = strlen(echoString)) > ECHOMAX)
	ErrorHandler("echo word too long");

	// TODO: Create UDP socket

	if ((my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	ErrorHandler("socket() failed");


	// TODO: Configure server address

	memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = PF_INET;
		echoServAddr.sin_port = htons(PORT);
		echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// TODO: Implement UDP communication logic
	if (sendto(my_socket, echoString, echoStringLen, 0, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) != echoStringLen)
	ErrorHandler("sendto() sent different number of bytes than expected");

	fromSize = sizeof(fromAddr);
	respStringLen = recvfrom(my_socket, echoBuffer, ECHOMAX, 0, (struct sockaddr*)&fromAddr, &fromSize);

	if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
	{
	fprintf(stderr, "Error: received a packet from unknown source.\n");
	exit(EXIT_FAILURE);
	}
	echoBuffer[respStringLen] = '\0';   // inutile con memset
	printf("Received: %s\n", echoBuffer);



	// TODO: Close socket
	closesocket(my_socket);
	printf("Client terminated.\n");
	clearwinsock();
	return 0;
} // main end
