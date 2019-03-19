#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/shm.h>
#include <string.h>
#include<bits/stdc++.h>
using namespace std;

int cus_sfds[100],driver_sfds[100];
int cs=0,drv=0;

int create_socket(int port){
	struct sockaddr_in server,client;
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	server.sin_family = AF_INET;         
    server.sin_port = htons(port);     
    server.sin_addr.s_addr = INADDR_ANY; 
    bzero(&(server.sin_zero),8); 
	
	if (bind(sfd, (struct sockaddr *)&server, sizeof(server))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	listen(sfd,5);
	return sfd;
}

int main(){

	struct sockaddr_in server,client;
	fstream d,cus;
	d.open("driver.txt",ios::in|ios::out);
	cus.open("customer.txt",ios::in|ios::out);

	while(!d.eof()){
		string s;
		getline(d,s,'\n');
		for(int i=0;i<s.length();i++){
			if(s[i]=='$'){
				string k = s.substr(0,i);
				int k1 = stoi(k,nullptr,10);
				driver_sfds[drv++] = create_socket(k1);
				break;
			}
		}
	}
	
	while(!cus.eof()){
		string s;
		getline(cus,s,'\n');
		for(int i=0;i<s.length();i++){
			if(s[i]=='$'){
				string k = s.substr(0,i);
				int k1 = stoi(k,nullptr,10);
				cus_sfds[cs++] = create_socket(k1);
				break;
			}
		}
	}
	
	d.close();
	cus.close();
	
	int sfd = create_socket(5001);
	
	char str[100];
	
	while(1){
		
		socklen_t sock_in = sizeof(struct sockaddr_in);
		
		int nsfd = accept(sfd,(struct sockaddr*)&client,&sock_in);
		
		int pid = fork();
		
		if(pid==0){
			close(sfd);
			recv(nsfd,str,100,0);
			printf("Client : %s\n",str);
			break;
		}
		
		else{
			close(nsfd);
		}
		
	}
	
	return 0;
}
	
