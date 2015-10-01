/*Server written by Alex Cameron 100861937*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFF_SIZE 256

int main(int argc, char *argv[]){
	//File decripters for the connection and client sockets
	int clientFD, connectionFD, portNumber, n;
	//length of the addresses
	socklen_t clientLength;

	char buffer[BUFF_SIZE];

	//Structs needed for sockets/bindings
	struct sockaddr_in server_addr, client_addr;
	
	//Create a socket
	connectionFD = socket(AF_INET, SOCK_STREAM, 0);
	if (connectionFD < 0){
		printf("Critical Error! Server cannot open a socket! ABORT!\n");
		exit(1);
	}  
	
	//Intialize address stucture
	memset((char *) &server_addr, 0, sizeof(server_addr));

	//Set the port number to random, or a provided number
	if(argc < 2){
//TODO:make random: //hint - bind to port 0 then figure out how to use 'getsockname();'
		portNumber = 20002;	
	}else{
		portNumber = atoi(argv[1]);
	}

	//Setup the server_addr
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	//Convert from host to network byte order
	server_addr.sin_port = htons(portNumber);

	//Bind the socket -returns -1 on error
	if( bind(connectionFD, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0 ){
		printf("Cirtial Error! could not bind to socket!\n");
		exit(1);
	}

	//Listen -and inform
	listen(connectionFD, 5);

	printf("Server is listening on port: %d\n", portNumber);

	//Main loop
	while(1){
		clientLength = sizeof(client_addr);
		clientFD = accept(connectionFD, (struct sockaddr *) &client_addr, &clientLength);
		if (clientFD < 0){
			printf("Critial Error! Cannot accept clients, might as well ABORT\n");
			exit(1);
		}
	
		printf("Client has connected! from: %s\n", inet_ntoa(client_addr.sin_addr));

		//Initialize the buffer
//		memset(buffer, 0, sizeof(buffer));
		
		//Inner main loop..for while a client is connected
		while(1){
			//receive messages from the client
			n = recv(clientFD, buffer, BUFF_SIZE - 1, 0);
			if (n < 0){
				printf("Cirital Error! Cannot get client requests! ABORT!\n");
				exit(1);
			}
			else if (n == 0) {
				printf("Connection closed. Listening.\n");
				break;
			}	
			//Set the end of the usable buffer then print it
			buffer[n] = '\0';
			printf("Message received from client: %s\n", buffer);

			if ((send(clientFD, buffer, strlen(buffer),0)) < 0){
				printf("Critical Error! Can't message client!\n");
				close(clientFD);
				break;
			}
			printf("Sending ----> %s\n", buffer);
		}
		//Close this client connection
		close(clientFD);
	}
	//Close socket
	close(connectionFD);
	return 0;
}

