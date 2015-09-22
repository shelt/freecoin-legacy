#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <netinet/tcp.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <stdio.h>  /* for perror() */
#include <stdlib.h> /* for exit() */

#define MAXPENDING 1

void die(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void handle(unsigned short quickAck, int clntSock)
{
    long long c_ts;                     /* current read timestamp */
    int value = 1;

    // Enable quickAck
    if (quickAck && setsockopt(clntSock, IPPROTO_TCP, TCP_QUICKACK, (char *)&value, sizeof(int)) < 0)
        die("TCP_QUICKACK failed");

    /* Send received string and receive again until end of transmission */
    while (recv(clntSock, (char*)&c_ts, sizeof(c_ts), 0) == sizeof(c_ts))      /* zero indicates end of transmission */
    {
        // Enable quickAck
        if (quickAck && setsockopt(clntSock, IPPROTO_TCP, TCP_QUICKACK, (char *)&value, sizeof(int)) < 0)
            die("TCP_QUICKACK failed");

        /* Echo message back to client */
        if (send(clntSock, (char*)&c_ts, sizeof(c_ts), 0) != sizeof(c_ts))
            die("send() failed to send timestamp");

        // Enable quickAck
        if (quickAck && setsockopt(clntSock, IPPROTO_TCP, TCP_QUICKACK, (char *)&value, sizeof(int)) < 0)
            die("TCP_QUICKACK failed");
    }

    close(clntSock);    /* Close client socket */
}

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned short quickAck;
    unsigned int clntLen;            /* Length of client address data structure */
    int value = 1;


    if (argc != 3)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port> <Quick Ack>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */
    quickAck = atoi(argv[2]);      /* Whether quick ack is enabled or not */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        die("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        die("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        die("listen() failed");

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        printf("Waiting for client...\n");

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            die("accept() failed");

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        if (setsockopt(clntSock, IPPROTO_TCP, TCP_NODELAY, (char *)&value, sizeof(int)) < 0)
            die("TCP_NODELAY failed");

        handle(quickAck, clntSock);
    }
    /* NOT REACHED */
}
