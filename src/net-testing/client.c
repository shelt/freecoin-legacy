#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netinet/tcp.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/time.h>

void die(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    int iterations, gap, i;          /* Number of timestamps to send, and gap between each send */
    struct timeval ts;
    long long c_ts, o_ts, delta, total = 0, max = 0, min = 1000000000;
    int value = 1;

    if (argc != 5)    /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Server IP> <Server Port> <Iterations> <Gap>\n", argv[0]);
        exit(1);
    }

    servIP = argv[1];              /* server IP address (dotted quad) */
    echoServPort = atoi(argv[2]);  /* server port */
    iterations = atoi(argv[3]);    /* number of timestamps to send */
    gap = atoi(argv[4]);           /* gap between each send */

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        die("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(echoServPort); /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        die("connect() failed");

    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&value, sizeof(int)) < 0)
        die("TCP_NODELAY failed");

    /* Give the server a chance */
    usleep(1000);

    /* Now for the given number of iterations */
    for(i = 0; i < iterations; ++i)
    {
        /* Generate the current timestamp */
        gettimeofday(&ts, NULL);
        c_ts = ts.tv_sec * 1000000LL + ts.tv_usec;

        //printf("sending %ld ", c_ts);

        /* Send this */
        if (send(sock, (char*)&c_ts, sizeof(c_ts), 0) != sizeof(c_ts))
            die("send() failed to send timestamp");

        /* Now read the echo */
        if (recv(sock, (char*)&o_ts, sizeof(o_ts), 0) != sizeof(o_ts))
            die("recv() failed to read timestamp");

        gettimeofday(&ts, NULL);
        c_ts = ts.tv_sec * 1000000LL + ts.tv_usec;

        /* Calculate the delta */
        delta = c_ts - o_ts;

        //printf(" -> received %ld %ld\n", o_ts, delta);

        if (i > 0)
        {
            /* Track max, min, sum */
            total += delta;
            max = (max < delta)? delta : max;
            min = (min > delta)? delta : min;
        }

        /* Now sleep */
        usleep(1000*gap);
    }
    --iterations;
    printf("iterations %d, avg %f, max %ld, min %ld\n", iterations, (total/(double)iterations), max, min);

    close(sock);
    exit(0);
}
