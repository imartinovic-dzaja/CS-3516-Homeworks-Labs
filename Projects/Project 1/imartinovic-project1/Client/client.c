#include <stdio.h> /* for printf() and fprintf() */
#include <stdlib.h> /* for atoi() and exit() */
#include <unistd.h> /* for close() */

#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <string.h> /* for memset() */

#define RCVBUFSIZE 2 /* Size of receive buffer */

void DieWithError(char *errorMessage); /* Error handling function */
void splitIntoHostAndFile(char* url, char* host, char* file); /*Function for separating hostname and filepath in a given URL*/
void constructHTTPGET(char* httpRequest, char* hostname, char* filepath); /*Function for constructing a HTTP GET request given hostname and filepath*/
void determineEndOfHTTPResponse(char* buffer, int* state);
void extractFilenameFromFilepath(char* filename, char* filepath);

int main(int argc, char *argv[])
{
    int sock; /* Socket descriptor */
    struct sockaddr_in* servAddr; /* HTTP server address */
    char* recBuffer;

    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;  /* Server info will be stored here after getaddrinfo()*/

    char* servPort; /* HTTPP server port */
    char* option;
    char* servURLorIP; /* Server URL or IP address (dotted quad) */

    int bytesRcvd, totalBytesRcvd; /* Bytes read in single recv() and total bytes read */


    if ((argc < 3) || (argc > 4)) /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s [<Option>] <Server IP OR URL> <Port> \n",
        argv[0]);
        exit(1);
    }

    if (argc == 3) {
        servURLorIP = argv[1]; /* First arg: server IP address (dotted quad) or URL */
        servPort = argv[2]; /* Second arg: Server port */
    }
    else
    {
        option = argv[1]; /*First arg: option*/
        servURLorIP = argv[2]; /* Second arg: server IP address (dotted quad) or URL */
        servPort = argv[3]; /* Third arg: Server port */
    }

    /*Setup the hints*/
    memset(&hints, 0, sizeof hints); /* make sure the struct is empty */
    hints.ai_family = AF_INET;       /* only IPv4 */
    hints.ai_socktype = SOCK_STREAM; /* TCP stream sockets */
    hints.ai_flags = AI_PASSIVE;     /* fill in my IP for me */

    /*Extracting hostname and filepath information*/
    char* host = (char*) malloc(sizeof(char)*strlen(servURLorIP));
    char* filepath = (char*) malloc(sizeof(char)*strlen(servURLorIP ));
    splitIntoHostAndFile(servURLorIP,host,filepath);

    /*Get server info*/
    if ((status = getaddrinfo(host, servPort, &hints, &servinfo)) != 0) {
        DieWithError(("getaddrinfo() failed: \n"));
        exit(1);
    }

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    servAddr = (struct sockaddr_in*) servinfo->ai_addr;

    /*Setup for RTT*/
    struct timeval timeBefore,timeAfter, timeDifference;
    gettimeofday(&timeBefore, NULL);

    /* Establish the connection to the HTTP server */
    if (connect (sock, (struct sockaddr *) servAddr, sizeof(*servAddr)) < 0)
        DieWithError("connect() failed\n");

    gettimeofday(&timeAfter, NULL);
    timersub(&timeAfter, &timeBefore, &timeDifference);

    /*Calculating RTT*/
    if(argc == 4 &&  (strcmp(argv[1], "-p") == 0)) {
        long seconds =(long)timeDifference.tv_sec;
        long microseconds =(long) timeDifference.tv_usec;

        long double milliseconds = seconds*1000 + (long double)microseconds/1000;
        printf("RTT is %Lf milliseconds\n", milliseconds);
    }

    /* Construct the actual HTTP request*/
    char* httpRequest = (char*) malloc(sizeof(char) * 10240); /*make sure to have enough space for a simple HTTP request*/
    constructHTTPGET(httpRequest, host, filepath);

    /*Get the massage length*/
    size_t httpRequestLen = strlen(httpRequest);

    /*Print the request message*/
    printf("Size of message is: %d\n", httpRequestLen);
    printf("Client sending message \n%s\n", httpRequest);

    /* Send the request to the server */
    if (send (sock, httpRequest, httpRequestLen, 0) != httpRequestLen)
        DieWithError("send() sent a different number of bytes than expected");

    /* Receive the server's response */
    /* Setup to print the echoed string */
    printf("Server response: \n");
    printf("----------------------\n");

    recBuffer = (char*) malloc(sizeof(char)*RCVBUFSIZE);
    int state = 0;
    FILE* htmlFile = NULL;
    char* filename = (char*) malloc(sizeof(char)* strlen(servURLorIP));
    extractFilenameFromFilepath(filename, filepath);

    if ((htmlFile = fopen(filename, "w")) == NULL){
        DieWithError("Error creating/opening html file to write");
    }

    while ((bytesRcvd = recv(sock, recBuffer, RCVBUFSIZE - 1, 0)) > 0)
    {
    /* Receive up to the buffer size (minus 1 to leave space for
    a null terminator) bytes from the sender */
        recBuffer[bytesRcvd] = '\0';
        printf("%s", recBuffer);
        determineEndOfHTTPResponse(recBuffer, &state);
        if (state == 4) { /*A state of 4 indicates we've encountered the sequence /r/n/r/n*/
            while ((bytesRcvd = recv(sock, recBuffer, RCVBUFSIZE - 1, 0)) > 0) {
                recBuffer[bytesRcvd] = '\0';
                fputs(recBuffer, htmlFile);
            }
        }
    }

    printf("\n"); /* Print a final linefeed */
    printf("Page saved to \"%s\"\n", filename);
    close (sock);
    /*Free allocated memory*/
    free(httpRequest);
    free(host);
    free(filepath);
    free(filename);
    free(recBuffer);
    fclose(htmlFile);
    exit(0);
}
/*Error handling function*/
void DieWithError(char *errorMessage) {
    printf(errorMessage);
    exit(1);
}
/*Helper function to help split URL into hostname and file path*/
void splitIntoHostAndFile(char* url, char* host, char* file) {
    int i = 0;

    /*Copy the hostname*/
    while(url[i] != '/' && url[i] != '\0') {
        host[i] = url[i];
        ++i;
    }
    host[i] = '\0'; /*Add null terminator to hostname*/

    /*Copy the file path*/
    if (url[i] == '\0') { /*if file path was not specified, set file path to root*/
        file[0] = '/';
        file[1] = '\0';
    }
    else if (url[i] == '/') { /*otherwise copy the file path*/
        int j = i;
        while(url[i] != '\0') {
            file[i-j] = url[i];
            ++i;
        }
        file[i] = '\0';
    }
    else {  /*URL had invalid format*/
        DieWithError("\nURL invalid format\n");
    }

}
/*Function for constructing the HTTP GET Request message*/
void constructHTTPGET(char* httpRequest, char* hostname, char* filepath) {
    strcpy(httpRequest, "GET ");
    strcat(httpRequest, filepath);
    strcat(httpRequest, " HTTP/1.1\r\n");
    strcat(httpRequest, "Host: ");
    strcat(httpRequest, hostname);
    strcat(httpRequest, "\r\n");
    strcat(httpRequest, "Connection: close\r\n");
    strcat(httpRequest, "Content-Type: text/html\r\n");
    strcat(httpRequest, "\r\n");
}
/*Function which determines whether we should start writing to a html file*/
void determineEndOfHTTPResponse(char* buffer, int* state){
    if(*buffer != '\r' && *buffer != '\n') {    //if the characters are not \r or \n reset state
        *state = 0;             //reset the state
        return;
    }
    else if(*buffer = '\n' && ((*state % 2) == 0)) { //if the character is \n and before it either came a different char or \r
        ++(*state);             //increment the state
        return;
    }
    else if(*buffer = '\r' && ((*state % 2) == 1)) { //if the character is \r and before it came \n
        ++(*state);             //increment the state
        return;
    }
    else if (*buffer = '\n') {//if there are two newlines in a row
        (*state) = 1;
        return;
    }
    else {  //if there is a different sequence of \r and \n that we don't care about
        (*state) = 0;
        return;
    }
}
/*Function for extracting the filename from a filepath*/
void extractFilenameFromFilepath(char* filename, char* filepath){
    if (strcmp(filepath, "/") == 0) {
        strcpy(filename, "index.html");
    }
    else {
        int n = strlen(filepath) - 1;
        int i = n;
        int j = 0;
        while(filepath[i] != '/') {
            --i;
        }
        ++i;
        while(i <= n) {
            filename[j] = filepath[i];
            ++i;
            ++j;
        }
    }
}

