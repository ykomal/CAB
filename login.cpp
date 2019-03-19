#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/shm.h>
#include <string.h>
#include<bits/stdc++.h>

using namespace std;

int main(){

	fstream d,cus;
	d.open("driver.txt",ios::in|ios::out);
	cus.open("customer.txt",ios::in|ios::out);
	
	map<int,string> driver,customer;

	while(!d.eof()){
		string s;
		getline(d,s,'\n');
		for(int i=0;i<s.length();i++){
			if(s[i]=='$'){
				string k = s.substr(0,i);
				int k1 = stoi(k,nullptr,10);
				string str = s.substr(i+1);
				driver[k1] = str;
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
				string str = s.substr(i+1);
				customer[k1] = str;
				break;
			}
		}
	}
	
	d.close();
	cus.close();
	
	int True = 1;
	struct sockaddr_in server,client;
	
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	
	setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&True,sizeof(int));
	
	server.sin_family = AF_INET;         
	server.sin_port = htons(5000);     
	server.sin_addr.s_addr = INADDR_ANY; 
	bzero(&(server.sin_zero),8); 
	
	bind(sfd,(struct sockaddr*)&server,sizeof(struct sockaddr));
	
	listen(sfd,5);
	char lid[100]="Login id : ",llid[100],pswd[100] = "Password : ",ppswd[100],suc[100] = "Success ",
						fail[100] = "Failure ... Retry",ses[100] = "Session timed out..";
	int c=0;
	while(1){
		
		socklen_t sock_in = sizeof(struct sockaddr_in);
		
		int nsfd = accept(sfd,(struct sockaddr*)&client,&sock_in);
		
		int pid = fork();
		int c=0,fl=0;
		if(pid==0){
			close(sfd);
			while(1){
			
				char str1[100];
				char str2[] = "Enter choice .. \n 1. Driver \n 2. Customer ";
				send(nsfd,str2,100,0);
				recv(nsfd,str2,100,0);
				int ch1 = atoi(str2);
				map<int,string> m;
				if(ch1==1){
					strcpy(str1,"driver.txt");
					m = driver;
				}
				else{
					strcpy(str1,"customer.txt");
					m  = customer;
				}
				
				char str[] = "Enter choice .. \n 1. Login \n 2. Register ";
				send(nsfd,str,100,0);
				recv(nsfd,str,100,0);
				int l;
				int ch = atoi(str);
				switch(ch){
					case 1:
						send(nsfd,lid,100,0);
						recv(nsfd,llid,100,0);
						l = atoi(llid);
						
						send(nsfd,pswd,100,0);
						recv(nsfd,ppswd,100,0);
						if(m[l]==string(ppswd)){
							send(nsfd,suc,100,0);
							fl=1;
						}
						else{
							send(nsfd,fail,100,0);
							c++;
						}
					break;
					
					case 2:
						while(1){
							send(nsfd,lid,100,0);
							recv(nsfd,llid,100,0);
							l = atoi(llid);
							send(nsfd,pswd,100,0);
							recv(nsfd,ppswd,100,0);
							if(m.find(l)==m.end()){
								fstream f;
								f.open(str1,ios::in|ios::out);
								f.seekg(ios::beg,ios::end);
								f<<llid<<"$"<<ppswd<<endl;
								printf("Added to map .. %d %s",l,ppswd);
								fflush(stdout);
								m[l] = string(ppswd);
								f.close();
								send(nsfd,suc,100,0);
								fl=1;
							}
							else{
								char st[] = "Already exists... sorry enter new details ";
								send(nsfd,st,100,0);
								continue;
							}
						}
					break;
					
				}
				if(c==3){
					send(nsfd,ses,100,0);
					close(nsfd);
					break;
				}
				if(fl==1){
					close(nsfd);
					break;
				}
			}
		}
		
		else{
			close(nsfd);
		}
		
	}
	
	return 0;
}
	
