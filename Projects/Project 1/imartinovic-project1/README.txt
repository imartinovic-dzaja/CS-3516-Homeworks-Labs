COMPILING
To compile the client simply cd to the client folder and type "make" into the terminal
To compile the server simply cd to the server folder and type "make" into the terminal

RUNNING CLIENT
To run the client type "./client [-option] URL port_number"
For example "./client -p www.google.com 80"
The client will receive the HTTP response message from www.google.com, the html file and
print the RTT required

If no option is provided:
For example "./client www.google.com 80"
The client will receive the HTTP response message from www.google.com and the html file
without printing the RTT

If the option is not -p the client will just ignore the option and proceed as if no option was provided

The client saves the file to index.html if no filepath is provided in the URL
If a filepath is provided in the URL the client will save the file to a file with an appropriate filename

To receive TMDG.html from the server application you must type: "./client linux.wpi.edu/TMDG.html server_port"

!!! If file path was not written correctly the server will return 404 Not Found !!!

RUNNING SERVER

To run the server type "./server port_number"
I've found that using port of 99999 would always work fine, unless you wish to use a different number, the code will work the same

The server checks only the first line of the HTTP Request:
If and only if the first line is "GET /TMDG.html HTTP/1.1/r/n/r/n" the server will respond with "HTTP/1.1 200 OK\r\n\r\n" and send the TMDG.html file
If the request method is HEAD, PUT or any other well defined request method the server will respond wiht "HTTP/1.1 "HTTP/1.1 501 Not Implemented\r\n\r\n"
If the request method is malformed the server will respond with "HTTP/1.1 400 Bad Request\r\n\r\n"
If the filepath specified is not "/TMDG.html" the server will respond with "HTTP/1.1 404 Not Found\r\n\r\n"

The server ingores any header lines sent by the client.

To terminate the server use CTRL+c

