/*
 * main.c
 *
 * UDP Server - Template for Computer Networks assignment
 *
 * Portable across Windows, Linux, and macOS
 */

#if defined WIN32
#include <winsock.h>
#include <ctype.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

#define NO_ERROR 0
#define INDIRIZZO_IP_SERVER "127.0.0.1"

// Compatibilità Windows
#if defined(WIN32)
    #ifndef INET_ADDRSTRLEN
        #define INET_ADDRSTRLEN 16
    #endif
    #ifndef NI_MAXHOST
        #define NI_MAXHOST 1025
    #endif
#endif

// Funzione strlwr per normalizzare stringhe
char* strlwr(char *s) {
    for (char *p = s; *p; p++) {
        *p = tolower((unsigned char)*p);
    }
    return s;
}

// Funzioni per generare valori meteo casuali
float get_temperature(void) { return -10.0f + (float)rand()/RAND_MAX*50.0f; } // -10 a 40
float get_humidity(void) { return 20.0f + (float)rand()/RAND_MAX*80.0f; }     // 20 a 100%
float get_wind(void) { return (float)rand()/RAND_MAX*100.0f; }                // 0 a 100 km/h
float get_pressure(void) { return 950.0f + (float)rand()/RAND_MAX*100.0f; }  // 950 a 1050 hPa

// Verifica se la città è valida
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

void ErrorHandler(char *errorMessage) {
    printf("%s\n", errorMessage);
}

void clearwinsock() {
#ifdef WIN32
    WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

    int port = SERVER_PROTOPORT;

    // Parsing argomenti
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            port = atoi(argv[i + 1]);
            if (port <= 0) {
                printf("Errore: porta non valida.\nUso: main.exe [-p porta]\n");
                return 1;
            }
            i++;
        } else {
            printf("Errore: argomento sconosciuto: %s\nUso: main.exe [-p porta]\n", argv[i]);
            return 1;
        }
    }

    printf("Porta selezionata: %d\n", port);

    // Inizializza Winsock su Windows
#if defined WIN32
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (result != NO_ERROR) {
        printf("Error at WSAStartup()\n");
        return 0;
    }
#endif

    int my_socket;
    struct sockaddr_in echoServAddr, echoClntAddr;
    unsigned int cliAddrLen;
    char echoBuffer[ECHOMAX];
    int recvMsgSize;

    // Creazione socket UDP
    if ((my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        ErrorHandler("socket() failed");

    // Configura indirizzo server
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_port = htons(port);
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind
    if ((bind(my_socket, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))) < 0)
        ErrorHandler("bind() failed");

    printf("In attesa di richieste...\n");

    weather_response_t resp;

    while (1) {
        cliAddrLen = sizeof(echoClntAddr);

        // Ricezione datagram
        recvMsgSize = recvfrom(my_socket, echoBuffer, ECHOMAX, 0,
                               (struct sockaddr *) &echoClntAddr, &cliAddrLen);
        if (recvMsgSize < 0) {
            ErrorHandler("recvfrom() failed");
            continue;
        }

        if (recvMsgSize != sizeof(weather_request_t)) {
            printf("Pacchetto di dimensione errata (%d byte). Ignorato.\n", recvMsgSize);
            continue;
        }

        weather_request_t req;
        int offset = 0;
        memcpy(&req.type, echoBuffer + offset, sizeof(char));
        offset += sizeof(char);
        memcpy(req.city, echoBuffer + offset, sizeof(req.city));
        offset += sizeof(req.city);
        req.city[63] = '\0';

        // --- LOG CLIENT con DNS lookup ---
        char client_name[NI_MAXHOST] = "Unknown";
        char client_ip[INET_ADDRSTRLEN];

#if defined(WIN32)
        strncpy(client_ip, inet_ntoa(echoClntAddr.sin_addr), sizeof(client_ip)-1);
        client_ip[sizeof(client_ip)-1] = '\0';
#else
        inet_ntop(AF_INET, &echoClntAddr.sin_addr, client_ip, sizeof(client_ip));
#endif

        struct hostent *host = gethostbyaddr((const char *)&echoClntAddr.sin_addr,
                                             sizeof(echoClntAddr.sin_addr), AF_INET);
        if (host != NULL) {
            snprintf(client_name, sizeof(client_name), "%s", host->h_name);
        } else {
            snprintf(client_name, sizeof(client_name), "%s", client_ip);
        }

        printf("Richiesta ricevuta da %s (ip %s): type='%c', city='%s'\n",
               client_name, client_ip, req.type, req.city);

        // ELABORAZIONE METEO
        strlwr(req.city);
        req.type = tolower((unsigned char)req.type);

        int cityStatus = findString(req.city);
        if (cityStatus == VALID_REQ) {
            resp.status = 0;
            switch (req.type) {
                case 't': resp.value = get_temperature(); break;
                case 'p': resp.value = get_pressure(); break;
                case 'h': resp.value = get_humidity(); break;
                case 'w': resp.value = get_wind(); break;
                default:
                    resp.status = 2; // tipo errato
                    resp.value = 0.0;
                    break;
            }
            resp.type = req.type;
        } else {
            resp.status = INVALID_CITY;
            resp.type = req.type;
            resp.value = 0.0;
        }

        // INVIO RISPOSTA
        char buffer[sizeof(uint32_t) + sizeof(char) + sizeof(float)];
        offset = 0;

        uint32_t net_status = htonl(resp.status);
        memcpy(buffer + offset, &net_status, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        memcpy(buffer + offset, &resp.type, sizeof(char));
        offset += sizeof(char);

        uint32_t temp;
        memcpy(&temp, &resp.value, sizeof(float));
        temp = htonl(temp);
        memcpy(buffer + offset, &temp, sizeof(float));
        offset += sizeof(float);

        ssize_t sent = sendto(my_socket, buffer, offset, 0,
                              (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr));
        if (sent != offset) {
            ErrorHandler("sendto() sent different number of bytes than expected");
        }
    }

    closesocket(my_socket);
    clearwinsock();
    return 0;
}
