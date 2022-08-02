#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#include "../sysprog-audio-1.5/audio.h"
#include "../include/lecteur.h"
#include "../include/socket.h"

#define MAXLINE 1024	//taille maximal du  buffer
#define PORT 8888	// le port sur lequel envoyer les donnees 

void parse_param(int argc, char** argv, char* name, struct in_addr* sin_addr, unsigned char* filter) {
	if(argc < 3) {
		perror("nombre de parametres invalide ");
		exit(1);
	}
	struct addrinfo* result;
	struct addrinfo hints;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	int err = getaddrinfo(argv[1], NULL, &hints, &result);
	if (err != 0) {
		fprintf(stderr, "%s\n", gai_strerror(err));
		exit(1);
	}
	*sin_addr = ((struct sockaddr_in*)result->ai_addr)->sin_addr;
	freeaddrinfo(result);
	
	strcpy(name, argv[2]);
	
	for(int i = 3; i < argc; i++) {
		if(strcmp("MONO", argv[i]) == 0) {
			*filter |= 0b01;
		}
		else if(strcmp("VOLUME", argv[i]) == 0) {
			*filter |= 0b010;
		}
		else if(strcmp("ECHO", argv[i]) == 0) {
			*filter |= 0b0100;
		}
	}
}

int main(int argc, char *argv[])
{
	char name[64];
	char buf[MAXLINE] = {0};
	struct in_addr sin_addr;
	unsigned char filter = 0b1000;
	parse_param(argc, argv, name, &sin_addr, &filter);
	
	printf("IP serveur trouver : %s Music asked: %s\n",inet_ntoa(sin_addr), name);
	
	struct sockaddr_in client;
	socklen_t clientlen = sizeof(client);
	ssize_t recvlen;
		
	fd_set sockfds;
	struct timeval timeout;
	
	int sockfd = verif_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	init_timeout_sock(sockfd, &sockfds, &timeout, 2,0);
	
	struct sockaddr_in serv;
	socklen_t servlen = sizeof(serv);
	init_sockaddr_in(&serv, AF_INET, htons(PORT), sin_addr.s_addr);
	
	char full[65];
	full[0] = filter;
	full[1] = '\0';
	strcat(full, name);
	
	verif_env(sockfd, full, strlen(full), 0, &serv, servlen);
	
	recvlen = verif_delai_recp(sockfd, buf, 6, &client, &clientlen, &sockfds, &timeout);
	
	verif_erreur((int*)buf, recvlen);
	
	printf("Metadata received %d %d %d\n",((int*)buf)[0], buf[4], buf[5]);
	
	int speakerfd = aud_writeinit(((int*)buf)[0], buf[4], buf[5]);
	
	if(speakerfd < 0) {
		perror("aud_writeinit failed");
		exit(1);
	}
	
	size_t bytes_to_read = (buf[4]/8) * buf[5] * (long)MULTISOCKER;
	
	while((recvlen = recvfrom(sockfd, buf, bytes_to_read,0, (struct sockaddr *)&client, &clientlen)) > 0) {
		write(speakerfd, buf , recvlen); // pas de verif du write
	}
	printf("Fin de la transmission\n");
	
	close(speakerfd);
	close(sockfd);
	return 0;
}