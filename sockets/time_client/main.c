#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define BUF_LEN 128
#define SOCKADDR_IN_ZEROS_COUNT 8
#define PORT_STR_LEN_MAX 6

typedef enum {
    IP_DEFAULT = AF_UNSPEC,
    IP_IPV4 = AF_INET,
    IP_IPV6 = AF_INET6,
} InternetProtocol;

typedef enum {
    TP_DEFAULT = 0,
    TP_TCP = IPPROTO_TCP, // 6
    TP_UDP = IPPROTO_UDP, // 17
} TransportProtocol;

typedef enum {
    AP_DEFAULT = 0,
    AP_DAYTIME = 13,
    AP_TIME = 37,
    AP_NTP = 123,
} ApplicationProtocol;

#define AS_DAYTIME "13"
#define AS_TIME    "37"
#define AS_NTP     "123"

/* https://github.com/lettier/ntpclient/blob/master/source/c/main.c */
typedef struct {
    uint8_t liVnMode;

    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;

    uint32_t rootDelay;
    uint32_t rootDispersion;
    uint32_t refId;

    uint32_t refTmSeconds;
    uint32_t refTmFraction;

    uint32_t origTmSeconds;
    uint32_t origTmFraction;

    uint32_t rxTmSeconds;
    uint32_t rxTmFraction;

    uint32_t txTmSeconds;
    uint32_t txTmFraction;
} NtpPacket;

int TransportToSocketType(TransportProtocol tProtocol) {
    switch (tProtocol) {
        case TP_TCP:
            return SOCK_STREAM;
        case TP_UDP:
            return SOCK_DGRAM;
        case TP_DEFAULT:
        default:
            return 0;
    }
}

void ApplicationToString(ApplicationProtocol aProtocol, char *dest, size_t len) {
    switch (aProtocol) {
        case AP_DAYTIME:
            strncpy(dest, AS_DAYTIME, len);
            return;
        case AP_TIME:
            strncpy(dest, AS_TIME, len);
            return;
        case AP_NTP:
        case AP_DEFAULT:
        default:
            strncpy(dest, AS_NTP, len);
            return;
    }
}

bool PrepareSocket(InternetProtocol iProtocol, TransportProtocol tProtocol,
 ApplicationProtocol aProtocol, const char *serverAddress, int *sockFd,
 struct addrinfo **servinfo) {
    char             portStr[PORT_STR_LEN_MAX];
    struct addrinfo  hints;
    int              getaddrInfoError;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = iProtocol;
    hints.ai_socktype = TransportToSocketType(tProtocol);
    hints.ai_protocol = tProtocol;

    ApplicationToString(aProtocol, portStr, PORT_STR_LEN_MAX);

    getaddrInfoError = getaddrinfo(serverAddress, portStr, &hints, servinfo);
    if (getaddrInfoError != 0) {
        if (getaddrInfoError == EAI_SYSTEM)
            perror("getaddrinfo");
        else
            fprintf(stderr, "getaddrinfo: %s\n",
             gai_strerror(getaddrInfoError));
        return false;
    }

    *sockFd = socket((*servinfo)->ai_family, (*servinfo)->ai_socktype,
     (*servinfo)->ai_protocol);
    if (*sockFd == -1) {
        perror("socket");
        freeaddrinfo(*servinfo);
        return false;
    }

    return true;
}

bool PrepareServer(int sockFd, struct addrinfo *servinfo) {
    switch (servinfo->ai_protocol) {
        case TP_TCP: {
            ssize_t connectResult = connect(sockFd, servinfo->ai_addr,
             servinfo->ai_addrlen);

            if (connectResult == -1) {
                perror("connect");
                return false;
            }
            break;
        }
        case TP_UDP:
        default: {
            ssize_t sendToResult = sendto(sockFd, " ", 1, 0, servinfo->ai_addr,
             servinfo->ai_addrlen);

            if (sendToResult == -1) {
                perror("sendto");
                return false;
            }
            break;
        }
    }

    return true;
}

bool GetTimeStringDaytime(int sockFd, struct addrinfo *servinfo, char *str,
 size_t len) {
    ssize_t receivedLen;
    bool    serverPrepared = PrepareServer(sockFd, servinfo);

    if (!serverPrepared)
        return false;

    /* TCP ignores address and len */
    receivedLen = recvfrom(sockFd, str, len, 0, servinfo->ai_addr,
     &servinfo->ai_addrlen);
    str[receivedLen] = '\0';

    return true;
}

bool GetTimeRfc868Time(int sockFd, struct addrinfo *servinfo, uint32_t *time) {
    bool    serverPrepared = PrepareServer(sockFd, servinfo);

    if (!serverPrepared)
        return false;

    /* TCP ignores address and len */
    recvfrom(sockFd, time, sizeof(uint32_t), 0, servinfo->ai_addr,
     &servinfo->ai_addrlen);
    *time = ntohl(*time);
    printf("%u\n", *time);

    return true;
}

/* https://tools.ietf.org/html/rfc868 */
time_t Rfc868ToUnix(uint32_t rfc868Time) {
    return rfc868Time - 2208988800;
}

bool GetTimeStringTime(int sockFd, struct addrinfo *servinfo, char *str,
 size_t len) {
    uint32_t   rfc868Time;
    bool       timeGotten = GetTimeRfc868Time(sockFd, servinfo, &rfc868Time);
    time_t     unixTime;
    struct tm *timeStruct;

    if (!timeGotten)
        return false;

    unixTime = Rfc868ToUnix(rfc868Time);
    timeStruct = localtime(&unixTime);
    strftime(str, len, "%a %Y-%m-%d %H:%M:%S %Z", timeStruct);

    return true;
}

bool GetTimeStringNtp(int sockFd, struct addrinfo *servinfo, char *str,
 size_t len) {
    uint32_t   rfc868Time;
    bool       timeGotten = GetTimeRfc868Time(sockFd, servinfo, &rfc868Time);
    time_t     unixTime;
    struct tm *timeStruct;

    if (!timeGotten)
        return false;

    unixTime = Rfc868ToUnix(rfc868Time);
    timeStruct = localtime(&unixTime);
    strftime(str, len, "%a %Y-%m-%d %H:%M:%S %Z", timeStruct);

    return true;
}

bool GetTimeString(InternetProtocol iProtocol, TransportProtocol tProtocol,
 ApplicationProtocol aProtocol, const char *serverAddress, char *str,
 size_t len) {
    int                 sockFd;
    struct addrinfo    *servinfo;
    struct sockaddr_in *sockarrdIn;
    bool                socketPrepared = PrepareSocket(iProtocol, tProtocol,
     aProtocol, serverAddress, &sockFd, &servinfo);
    bool                timeReceived = false;

    if (!socketPrepared)
        return false;

    sockarrdIn = (struct sockaddr_in *)servinfo->ai_addr;
    switch (ntohs(sockarrdIn->sin_port)) {
        case AP_DAYTIME:
            timeReceived = GetTimeStringDaytime(sockFd, servinfo, str, len);
            break;
        case AP_TIME:
            timeReceived = GetTimeStringTime(sockFd, servinfo, str, len);
            break;
        case AP_NTP:
        default:
            timeReceived = GetTimeStringNtp(sockFd, servinfo, str, len);
            break;
    }

    freeaddrinfo(servinfo);

    return timeReceived;
}

int main(int argc, char **argv) {
    char buffer[BUF_LEN];

    GetTimeString(IP_IPV4, TP_TCP, AP_TIME, "time.nist.gov", buffer,
     BUF_LEN);

    printf("%s\n", buffer);

    return 0;
}
