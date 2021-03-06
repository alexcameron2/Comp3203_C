/*Client written by Alex Cameron 100861937*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_LENGTH 80
#define BUFF_SIZE 256

int main(int argc, char *argv[]){
	int connectionFD, portNumber, n;
	struct sockaddr_in server_addr;
	struct hostent *server;

	char buffer[256];

	//need for getlone
	unsigned int len;
	char *buff = NULL;
	
	if(argc < 3){
		printf("Critical error! Must provide ip and port!\n");
		exit(1);
	}

	portNumber = atoi(argv[2]);
	
	//Create the socket	
	connectionFD = socket(AF_INET, SOCK_STREAM, 0);
	if (connectionFD < 0) {
		printf("Critical error! Cannot create socket! ABORT\n");
		exit(1);
	}
	
	//Get the host name entered
	server = gethostbyname(argv[1]);
	if (server == NULL){
		printf("Critial error! Host name entered cannot be found! ABORT!\n");
		exit(1);
	}

	memset((char *) &server_addr,0, sizeof(server_addr));
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portNumber);

	server_addr.sin_addr = *((struct in_addr *)server->h_addr);

	if( connect(connectionFD, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0 ){
		printf("Critial error! could not connect to the server! ABORT\n");
		exit(1);
	}
//	memset(buffer, 0, sizeof(buffer));
	//Main loop
	while(1){
		printf("Enter message to server: ");
		fflush(stdout);
		//get user input into the buffer
		getline(&buff, &len, stdin);
	
		if( (send(connectionFD, buff, strlen(buff), 0)) < 0){
			printf("Critical error! Can't send to server, might as well ABORT!\n");
			exit(1);
		}
		else {
			printf("Sending ---->: %s\n", buff);
			n = recv(connectionFD, buff, BUFF_SIZE - 1, 0);
			if(n <=0){
				printf("Server stopped? Critical ERROR?! ABORT!");
				break;
			}

			buff[n] = '\0';
			printf("Message from server: %s\n", buff);
		}

	}
	free(buff);
	close(connectionFD);
	return 0;
}

