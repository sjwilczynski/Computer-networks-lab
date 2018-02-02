//Stanisław Wilczyński 272955
#ifndef WRAP
#define WRAP

#include <stdexcept>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <unistd.h> 
#include <assert.h>
#include <sys/time.h>

ssize_t my_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
int my_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
void my_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
int my_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int my_socket(int domain, int type, int protocol);
void my_inet_pton(int af, const char *src, void *dst);

#endif