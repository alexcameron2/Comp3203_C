/*Server written by Alex Cameron 100861937*/

//common stuff
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//connection stuff
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>

//system stuff
#include<dirent.h>

//DEFINITIONS
#define BUFF_SIZE 1025



//Helper function that returns a string representing the pwd
char * showPWD(){
	char *returnValue;
	returnValue = (char *)malloc(7*sizeof(char));
	strcpy(returnValue, "pwd-> ");
	char pwd[256];
	if (getcwd(pwd, sizeof(pwd)) != NULL){
		asprintf(&returnValue, "%s", pwd);
	}	
return returnValue;
}

//Helper function that returns a string of all the files in a directory in a neat list. 
char * showFiles(){
printf("Entered showDirectory!\n");
	int index = 0;
	char *returnValue;
	returnValue = (char *)malloc(15*sizeof(char));
	strcpy(returnValue, "\nFILES LIST:\n ");
	char *temp;
	DIR *dir;
	struct dirent *directory;
	dir = opendir(".");
	if (dir){
		while((directory = readdir(dir)) != NULL){
			asprintf(&temp, "%s", returnValue);
			if(index % 5 == 0){
				asprintf(&returnValue, "%s %s\n", temp, directory->d_name);	
			}		
			else{
				asprintf(&returnValue, "%s %s", temp, directory->d_name);		
			}
		}

		closedir(dir);
	}
printf("Returning from showDirectory: %s\n", returnValue);
free(temp);
return returnValue;
}

char * makeDirectory(const char *fn, const char *p){
	//struct needed to check if a file exists easily	
	struct stat st = {0};
	
	char *path;
	char *final;
	char *cutter;
	if(fn == NULL){
		return "Error! formart for mkdir is: mkdir File_Name Path\n";
	}
	if (p == NULL){
		path = showPWD();
	}
	else {
		asprintf(&path, "%s", p);
	}
	
	asprintf(&final, "%s/%s", path, fn);
	//remove the final character
	final[strlen(final) - 1] = 0;
	//Attemp to make the directory
	if (stat (final, &st) == -1){
		if ( mkdir(final, 0777) < 0){
			return "Error! could not create directory specified!\n";
		}
		else{
			return "Directory created!\n";
		}
	}
} 

//Function that identifies what opperation the client wants the server to perform
char * identifyRequest(const char *req){
	char * commands[] = {"ls", "pwd", "cd", "mkdir" };

	char *r;
	//Need this struct to check if a file exists easily
	struct stat st ={0};

	//Copy the request the client buffered in so it can be manipulated
printf("handling request..buffer= %s\n\n", req);
	char request[BUFF_SIZE];
printf("Attempitng strpy()\n");
	memset(request, '\0', sizeof(request));
	strcpy(request, req);
printf("handling request..buffer= %s ..request = %s\n", req, request);
	//Split the string into varibles	
	char *command = strtok(request, " \n");
printf("Got the first argument: %s\n", command);
	char *filename = strtok(NULL, " ");
	char *path = strtok(NULL, " ");
printf("Got the first argument: %s\n", command);

	//Compare the first given argument to a list of commands
	int index = -1;
	int i=0;
	while(i < 5){
printf("trying to compare command: -%s-  -to-> commands[%d]: -%s-\n", command, i, commands[i]);
		if(strcmp(commands[i], command) == 0) {
printf("Command found!");
		index = i;
		break;
		}
		i++;
	}	
	
printf("INDEX =: %d\n", index);
	switch(index) {
		case 0:
			r = showFiles();
			break;

		case 1:
			r = showPWD();
			break;

		case 2:
		case 3:
			r = makeDirectory(filename, path);
			break;
			
		default:
			r = "identiyRequest()-> Command Not Found\n";
	}
	
	
return r;
}


int main(int argc, char *argv[]){
	//File decripters for the connection and client sockets
	int clientFD, connectionFD, portNumber, n;
	//length of the addresses
	socklen_t clientLength;

	char buffer[BUFF_SIZE];
	char *p;
	p = (char *)malloc(100*sizeof(char));
	strcpy(p, "\np: ");
	int temp;
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
			printf("Full line received from client: %s\n", buffer);
			temp = asprintf(&p, "%s", identifyRequest(buffer));
//			memset (buffer, '\0', sizeof(buffer);
//			if (p == NULL){
//				printf("p was null..no ls message back.");
//			}
//			else {
//				strcpy(buffer, p);
//			}
			if (p != NULL){
			strcpy(buffer, p);
			}

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

