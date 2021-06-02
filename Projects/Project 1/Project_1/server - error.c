#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define RCVBUFSIZE 32

void DieWithError(char *errorMessage); /* Error handling function */
void HandleTCPClient(int clntSocket); /* TCP client handling function */
void extractMethodAndFilepathFromRequest(char* HTTPMethod, char* filepath, char* HTTPRequest);/*function for extracting HTTP method and filepath from HTTP request*/
void constructHTTPResponse(char* HTTPResponse, char* HTTPmethod, char* filepath); /*Function for constructing appropriate HTTP response*/

int main(int argc, char *argv[])
{
    int servSock; /*Socket descriptor for server */
    int clntSock; /* Socket descriptor for client */
    struct sockaddr_in servAddr; /* Local address */
    struct sockaddr_in clientAddr; /* Client address */
    unsigned short servPort; /* Server port */
    unsigned int clntLen; /* Length of client address data structure */

    printf("Server has started\n");

    if (argc != 2) /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }
    servPort = atoi(argv[1]); /* First arg: local port */

    /* Create socket for incoming connections */
    if ((servSock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    printf("Socket created\n");

    /* Construct local address structure */
    memset(&servAddr, 0, sizeof(servAddr)); /* Zero out structure */
    servAddr.sin_family = AF_INET; /* Internet address family */
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    servAddr.sin_port = htons(servPort); /* Local port */
    /* Bind to the local address */
    if (bind (servSock, (struct sockaddr *) &servAddr, sizeof(servAddr))<0)
        DieWithError("bind() failed");

    printf("Socket bound\n");
    /* Mark the socket so it will listen for incoming connections */
    if (listen (servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");
    printf("Listening started\n");

    for (;;) /* Run forever */
    {
    /* Set the size of the in-out parameter */
        clntLen = sizeof(clientAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept (servSock, (struct sockaddr *) &clientAddr, &clntLen)) < 0)
            DieWithError("accept() failed");

        printf("Client accepted\n");


    /* clntSock is connected to a client! */
        printf("Handling client %s\n", inet_ntoa(clientAddr.sin_addr));

        HandleTCPClient(clntSock);


        close(clntSock);
        exit(0);
    }
close(servSock);
}

/*Error Handling Function*/
void DieWithError(char *errorMessage) {
    printf("%s\n",errorMessage);
    exit(1);
}
/*Function for handling TCP Client*/
void HandleTCPClient(int clntSocket) {
    int bytesRcvd;
    int totalBytesRcvd = 0;
    char* rcvBuffer = (char*) malloc(sizeof(char)*RCVBUFSIZE);

    printf("Inside HandleTCPClient \n");

    char* HTTPRequest = (char*) malloc(sizeof(char) * 10240);
    HTTPRequest[0] = '\0';

    int i = 0;
    while (
           (i<10) && (
           (printf("first\n")==10) ||
           ((bytesRcvd = recv(clntSocket, rcvBuffer, RCVBUFSIZE - 1, 0)) > 0) ||
            (printf("second\n")==10))
           )
    {
    ++i;
    printf("WHASSUP\n");

    /* Receive up to the buffer size (minus 1 to leave space for
    a null terminator) bytes from the sender */
        rcvBuffer[bytesRcvd] = '\0'; /* Terminate the string! */

        if (bytesRcvd == 0) {
            break;
        }
        strcat(HTTPRequest, rcvBuffer);
        /*if(bytesRcvd < (RCVBUFSIZE-1)){
            break;
        }*/
    }
    printf("Server has received message %s\n", HTTPRequest); /* Print the echo buffer */

    char* HTTPResponse = (char*) malloc(sizeof(char) * 1024);
    char* HTTPMethod = (char*) malloc(sizeof(HTTPRequest));
    char* filepath = (char*) malloc(sizeof(HTTPRequest));

    extractMethodAndFilepathFromRequest(HTTPMethod, filepath, HTTPRequest);
    printf("HTTPmethod: %s\tfilepath: %s\n", HTTPMethod, filepath);
    constructHTTPResponse(HTTPResponse, HTTPMethod, filepath);

    send(clntSocket, HTTPResponse, strlen(HTTPResponse), 0);

    free(HTTPResponse);
    free(HTTPMethod);
    free(filepath);
}
/*Function for extracting the HTTP Method and filepath from a HTTP Request*/
void extractMethodAndFilepathFromRequest(char* HTTPMethod, char* filepath, char* HTTPRequest){
    int i = 0;
    int n = strlen(HTTPRequest);
    /*Extract the HTTP Method*/
    while (HTTPRequest[i] != ' '){
        HTTPMethod[i] = HTTPRequest[i];
        if(i == n) {
            break;
        }

        /*Watch out if the HTTP Request ends abruptly*/
        if(HTTPRequest[i] == '\0'){
            filepath[0] = '\0';
            return;
        }
        ++i;
    }
    ++i;
    HTTPMethod[i] = '\0';

    /*Extract the filepath*/
    int j = 0;
    while(HTTPRequest[i] != ' ') {
        filepath[j] = HTTPRequest[i];
        /*Watch out if the HTTP Request ends abruptly*/
        if(HTTPRequest[i] == '\0'){
            filepath[j] = '\0';
            return;
        }
        ++i;
        ++j;
    }
    filepath[j] = '\0';
}

/*Function for constructing appropriate HTTP response*/
void constructHTTPResponse(char* HTTPResponse, char* HTTPmethod, char* filepath) {

    if(strcmp(HTTPmethod, "GET")!= 0) {
        int isHEAD = (strcmp(HTTPmethod, "HEAD") == 0);
        int isPUT = (strcmp(HTTPmethod, "PUT") == 0);
        int isPOST = (strcmp(HTTPmethod, "POST") == 0);
        int isDELETE = (strcmp(HTTPmethod, "DELETE") == 0);
        int isCONNECT = (strcmp(HTTPmethod, "CONNECT") == 0);
        int isOPTIONS = (strcmp(HTTPmethod, "OPTIONS") == 0);
        int isTRACE = (strcmp(HTTPmethod, "TRACE") == 0);

        if(isHEAD || isPUT || isPOST || isDELETE || isCONNECT || isOPTIONS || isTRACE) {
            strcpy(HTTPResponse, "HTTP/1.1 501 Not Implemented");
            return;
        }
        else {
            strcpy(HTTPResponse, "HTTP/1.1 400 Bad Request");
            return;
        }
    }
    else {
        if(strcmp(filepath, "/TMDG.html") == 0) {
            strcpy(HTTPResponse, "HTTP/1.1 200 OK");
            return;
        }
        else {
            strcpy(HTTPResponse, "HTTP/1.1 404 Not Found");
            return;
        }
    }

}

