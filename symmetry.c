// Symmetry: Bracket counting IRC daemon
//  LICENSE: MPL2

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define HOST "irc.freenode.net"
#define PORT "6667"

#define MAXBUF 4096

typedef enum {DEBUG, INFO, WARN, ERROR, FATAL} loglevel;
const char *log_s[] = {"D", "I", "W", "E", "F"};

#define die(...) wlog(FATAL, __VA_ARGS__)

void wlog(loglevel level, const char *s, ...)
{
    char fmt[strlen(s)+5];
    snprintf(fmt, sizeof(fmt), "%s: %s\n", log_s[level], s);
    va_list args;
    va_start(args, s);
    vfprintf(stderr, fmt, args);
    va_end(args);
    if ( level >= FATAL ) exit(1);
}

int irc_connect(char *host, char *port)
// Fill in addrinfo, and format ip to string
{
    int sockfd, retval;
    struct addrinfo hints;
    struct addrinfo *ai, *p;
    char ip[INET6_ADDRSTRLEN];
  
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // IPv4 or 6 (AF_INET or AF_INET6)
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // Autodetect local host
    if ( (retval = getaddrinfo(host, port, &hints, &ai)) != 0 )
        die("Failed to resolve host: %s (%s)", HOST, gai_strerror(retval));

    for ( p = ai; p != NULL; p = p->ai_next )
    {
	void *server; // sockaddr_in or sockaddr_in6
	if ( p->ai_family == AF_INET )
	{ // IPv4
	    struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
	    server = &(ipv4->sin_addr);
	}
	else
	{ // IPv6
	    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
	    server = &(ipv6->sin6_addr);
	}
	inet_ntop(p->ai_family, server, ip, sizeof(ip));

	wlog(DEBUG, "Connecting to %s (%s)... ", HOST, ip);

	if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0 )
	{ 
	    wlog(DEBUG, "Could not create socket");
	    continue;
	}

	if ( (retval = connect(sockfd, p->ai_addr, p->ai_addrlen)) < 0 )
	    die("Connection to %s failed [%d]", retval);
    }
    if ( p == NULL ) 
	die("Could not resolve host: %s", HOST);
    else wlog(INFO, "Connected!");

    freeaddrinfo(ai);
        
    return sockfd;
}

int main(int argc, char* argv[])
{
    int fd;
    ssize_t len;
    char buffer[MAXBUF];
    
    switch ( argc )
    {
    case 0:
	fd = irc_connect(HOST, PORT);
	break;
    case 1:
	fd = irc_connect(argv[0], PORT);
	break;
    case 2:
    default:
	fd = irc_connect(argv[0], argv[1]);
	break;
    }

    len = recv(fd, buffer, MAXBUF, 0);
    buffer[len] = '\0';
    while ( len > 0 )
    {
        //printf("=%03d=> %s", len, buffer);
        len = recv(fd, buffer, MAXBUF-1, 0);
        buffer[len] = '\0';

	char *end, *str = buffer;
	while ( str )
	{
	    end = strchr(str, '\n');
	    if ( end ) *end = '\0';
	    printf("==> %s\n", str);
	    str = end ? end+1 : NULL;
	}
    }
    if ( len == -1 )
	die("Error reading from socket");
    close(fd);
    return 0;
}
