#ifndef SOCKET_FILE
#define SOCKET_FILE

#define MULTISOCKER 64
#include <sys/socket.h>
#include "../include/lecteur.h"


int verif_socket(int domain, int type, int protocol);

void verif_bind(int sockfd, const struct sockaddr_in *addr,socklen_t addrlen);

void init_sockaddr_in(struct sockaddr_in *addr, short int sin_family, unsigned short sin_port, unsigned long s_addr);

void envoy_donnee(int sockfd, char *buf, struct sockaddr_in *client, size_t len, struct Son *s, unsigned char filter);

void envoy_erreur(int sockfd, int* buf, struct sockaddr_in* client, size_t len, int errno);

void envoy_son(Son *s, int sockfd, int multi, unsigned char filter, float lvl, struct sockaddr_in *client, socklen_t clientlen);

void init_timeout_sock(int sockfd, fd_set* set, struct timeval* timeout, time_t tv_sec, suseconds_t tv_usec);

ssize_t verif_env(int sockfd, const void *buf, size_t len, int flags, struct sockaddr_in *dest_addr, socklen_t addrlen);

ssize_t verif_delai_recp(int sockfd, void* buf, size_t len, struct sockaddr_in *client, socklen_t *clientlen, fd_set *sockfds, struct timeval *timeout);

void verif_erreur(int* buf, int recvlen);



#endif