#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "net.h"
#include "symmetry.h"

enum {WAITING, REGISTERED} state = WAITING;

void handle(int fd, char *line)
{
    char *hostmask = 0, *command = 0, *params = 0;
    wlog(MSG, "==> %s", line);
    if ( state == WAITING )
    {
	reply(fd, "USER symmetry 8 * :Symmetry");
	reply(fd, "NICK symmetry");
	state = REGISTERED;
	return;
    }

    hostmask = line; // w/ :
    command = strchr(hostmask, ' ');
    *command = '\0';
    command++;
    params = strchr(command, ' ');
    if ( params )
    {
	*params = '\0';
	params++;
    }

    if ( strcmp("ERROR", hostmask) == 0 )
    {
	wlog(DEBUG, "Server closed the connection");
	exit(0);
    }
    else if ( strcmp("PING", hostmask) == 0 )
    {
	reply(fd, "PONG %s", command);
    }
    else if ( strcmp("001", command) == 0 )
    {
	reply(fd, "JOIN #koyomi");
    }
}
