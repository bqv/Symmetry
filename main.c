// Symmetry: Bracket counting IRC daemon
//  LICENSE: MPL2

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "symmetry.h"
#include "logging.h"
#include "net.h"
#include "config.h"

#define MAXBUF 4096

int main(int argc, char* argv[])
{
    int fd;
    ssize_t len;
    char buffer[MAXBUF];
    
    switch ( argc )
    {
    default:
    case 0:
    case 1:
	fd = irc_connect(HOST, PORT);
	break;
    case 2:
	fd = irc_connect(argv[1], PORT);
	break;
    case 3:
	fd = irc_connect(argv[1], argv[2]);
	break;
    }

    len = recv(fd, buffer, MAXBUF-1, 0);
    buffer[len] = '\0';
    while ( len > 0 )
    {
	char *end, *str = buffer;
	while ( str )
	{
	    end = strchr(str, '\n');
	    if ( end ) *(end-1) = '\0';
	    handle(fd, str);
	    str = end ? ( *(end+1) == '\0' ? NULL : end+1 ) : NULL;
	}
        len = recv(fd, buffer, MAXBUF-1, 0);
        buffer[len] = '\0';
    }
    if ( len == -1 )
	die("Error reading from socket");
    close(fd);
    return 0;
}
