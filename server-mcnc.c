#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/shm.h> 
#include <sys/time.h> 
	
#define TRUE 1 
#define FALSE 0 
#define PORT1 5004
#define PORT2 5005
#define PORT3 5006 
	
int main(int argc , char *argv[]) 
{ 
	int opt = TRUE; 
	int master_socket[3] , addrlen , new_socket , client_socket[30] , 
		max_clients = 30 , activity, i , valread , sd; 
	int max_sd=-1; 
	struct sockaddr_in address; 
		
	char buffer[1025]; 
	fd_set readfds; 

	key_t key;
	int shmid;
	 
	key = 5678;
	shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
	int *v1 = shmat(shmid, NULL, 0);
		
	//a message 
	char *message = "Welcome Client .. \n"; 
	
	//initialise all client_socket[] to 0 so not checked 
	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	} 
		
	for(int i=0;i<3;i++){
		if( (master_socket[i] = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
		{ 
			perror("socket failed"); 
			exit(EXIT_FAILURE); 
		} 
		
		if( setsockopt(master_socket[i], SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
			sizeof(opt)) < 0 ) 
		{ 
			perror("setsockopt"); 
			exit(EXIT_FAILURE); 
		} 
	}
	
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT1 ); 
		
	if (bind(master_socket[0], (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_port = htons( PORT2 ); 
		
	if (bind(master_socket[1], (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_port = htons( PORT3 ); 
		
	if (bind(master_socket[2], (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	for(int i=0;i<3;i++){
		if (listen(master_socket[i], 3) < 0) 
		{ 
			perror("listen"); 
			exit(EXIT_FAILURE); 
		} 
	}
	 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
		
	while(TRUE) 
	{ 
		FD_ZERO(&readfds); 
		
		for(int i=0;i<3;i++){
			FD_SET(master_socket[i], &readfds); 
			if(max_sd<master_socket[i])
				max_sd = master_socket[i]; 
		}
		
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			sd = client_socket[i]; 
				
			if(sd > 0) 
				FD_SET( sd , &readfds); 
				
			if(sd > max_sd) 
				max_sd = sd; 
		} 
	
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
		char* args[] = {"./S1","./S2","./S3",NULL}; 
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
		
		for(int i=0;i<3;i++){
			if (FD_ISSET(master_socket[i], &readfds)) 
			{ 
				if ((new_socket = accept(master_socket[i],(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
				{ 
					perror("accept"); 
					exit(EXIT_FAILURE); 
				} 
				 
			
				//send new connection greeting message 
				if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
				{ 
					perror("send"); 
				} 
					
				puts("Welcome message sent successfully");
				*v1 = new_socket;
				int c = fork();
				if(c>0)
					execv(args[i],args);
			} 
		}	
		//else its some IO operation on some other socket 
		
	} 
		
	return 0; 
} 

