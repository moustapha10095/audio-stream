#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../sysprog-audio-1.5/audio.h"
#include "../include/lecteur.h"
#include "../include/socket.h"

#include <sys/timerfd.h>                           
#include <sys/wait.h>
#include <sys/select.h>

#define MAXLINE 1024
#define PORT 8888 // udp port 
#define MAX_PID 3


int main(void)
{
	// initialisations du serveur 
	char buf[MAXLINE] = {0};
	char name[64] = {"\0"};
	char full[65] = {"\0"};
	unsigned filter = 0;
	// description du socket client  qui va etre envoyer 
	struct sockaddr_in client;
	socklen_t clientlen = sizeof(client);
	size_t lenrcv;
	
	int ipid = 0;
	pid_t pid = 1;
	int status;
	
	fd_set sockfds;
	struct timeval timeout;
	
	int sockfd = verif_socket(AF_INET, SOCK_DGRAM, 0);
	
	struct sockaddr_in serv;
	init_sockaddr_in(&serv, AF_INET, htons(PORT), htonl(INADDR_ANY));
	
	verif_bind(sockfd, &serv, sizeof(serv));
	
	init_timeout_sock(sockfd, &sockfds, &timeout, 2, 0);
	
	printf("Server open with ip: %s Port: %d\n", inet_ntoa(serv.sin_addr),ntohs(serv.sin_port));
	// Fin  d'initialisations du serveur
	
	while(1) {
		
		lenrcv = verif_delai_recp(sockfd, full, 64, &client, &clientlen, &sockfds, &timeout);
		
		filter = full[0];
		strcpy(name, (full+1));
		
		usleep(2);
		
		if(lenrcv > 0) {
			printf("Name received: %s Client: %s Port: %hu \n",name,inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			
			// Gestion des erreurs
			if(ipid == MAX_PID) {
				printf("Max fork achieved, refusing request...\n");
				envoy_erreur(sockfd, (int*)buf, &client, clientlen, 503);
			}
			else if(access(name, F_OK ) == -1) {
				printf("Error 404 File not found, refusing request...\n");
				envoy_erreur(sockfd, (int*)buf, &client, clientlen, 404);
			}
			else {
				printf("File exists - PID ok\n");
				ipid++;
				if((pid = fork()) < 0) {
					perror("fork failed");
				}
				
				if(pid > 0) {
					bzero((char *) &name, sizeof(name));
				}
			}
		}
		
		if(pid == 0) {
			Son s;
			nouveauSon(&s, name);
			
			if(available_filter(&s,filter) == 0) {
				envoy_erreur(sockfd, (int*)buf, &client, clientlen, 405);
				exit(0);
			}
			
			envoy_donnee(sockfd, buf, &client, clientlen, &s, filter);
			
			usleep(2);
			envoy_son(&s, sockfd, MULTISOCKER, filter, 0.8, &client, clientlen);
			sendto(sockfd,buf, 0, 0,(struct sockaddr*)&client,clientlen);
			exit(0);
		}
		
		if(waitpid(0, &status, WNOHANG) > 0) {
			printf("One zombie died\n");
			ipid--;
		}
	}

	close(sockfd);
	return 0;
}