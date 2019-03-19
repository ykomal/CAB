#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


	
int main(){

	struct sockaddr_in server,client;
	struct hostent *host;
	
	char buf[100],hello[100]="Hello";
	
	struct sockaddr_in server_addr;
	host = gethostbyname("127.0.0.1");
	server.sin_family = AF_INET;     
    server.sin_addr = *((struct in_addr *)host->h_addr);
	server.sin_port = htons( 5000 ); //login-port
	
	host = gethostbyname("127.0.0.1");
	server_addr.sin_family = AF_INET;     
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&(server.sin_zero),8); 
	connect(sfd,(struct sockaddr*)&server,sizeof(struct sockaddr));	
	
	while(1){
		
		recv(sfd,buf,100,0);
		printf("%s",buf);
		if(strcmp(buf,"Success ")==0){
			printf("Logged in..");
			close(sfd);
			break;
		}
		printf("\n");
		if(strcmp(buf,"Failure ... Retry")==0||strcmp(buf,"Already exists... sorry enter new details ")==0){
			continue;
		}
		if(strcmp(buf,"Session timed out..")==0){
			close(sfd);
			break;
		}
		scanf("%s",buf);
		if(strcmp(buf,"Login id : ")==0){
			int lid = atoi(buf);
			server_addr.sin_port = htons(lid);
		}
		send(sfd,buf,100,0);
		
	}
	
	int ssfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&(server_addr.sin_zero),8); 
	connect(ssfd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr));	
	send(ssfd,hello,100,0);
			
	return 0;
}
	
