#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "net.h"
#include "symmetry.h"
#include "config.h"

enum {WAITING, REGISTERED} state = WAITING;

enum {ROUND, SQUARE, CURLY, ANGLE};
long long cnt[] = {0,0,0,0};

void handle(int fd, char *line)
{
    char *hostmask, *command, *param, *args;
    wlog(MSG, "==> %s", line);
    if ( state == WAITING )
    {
        reply(fd, "USER "NICK" 8 * :Symmetry");
        reply(fd, "NICK "NICK);
        #ifdef PASS
            reply(fd, "PASS "PASS);
        #endif
        state = REGISTERED;
        return;
    }

    hostmask = line; // w/ :
    command = strchr(hostmask, ' ');
    if ( strcmp("ERROR", hostmask) == 0 )
    {
        wlog(DEBUG, "Server closed the connection");
        exit(0);
    }
    else if ( command )
    {
        *command = '\0';
        param = strchr(++command, ' ');
        if ( strcmp("PING", hostmask) == 0 )
            reply(fd, "PONG %s", command);
        else if ( param )
        {
            *param = '\0';
            args = strchr(++param, ' ');
            if ( strcmp("001", command) == 0 )
                reply(fd, "JOIN #koyomi");
            else if ( args ) 
            {
                *args = '\0';
                args = *(args+1) == ':' ? args+2 : args+1;
                if ( strcmp("INVITE", command) == 0 )
                    reply(fd, "JOIN %s", args);
                else if ( strcmp("PRIVMSG", command) == 0 )
                {
                    char query[] = "@"NICK;
                    if ( strncmp(query, args, sizeof(query)) == 0 )
                        reply(fd, "PRIVMSG %s :\x02" "Round\x02: %lld - "
                                              "\x02" "Square\x02: %lld - "
                                              "\x02" "Curly\x02: %lld - "
                                              "\x02" "Angle\x02: %lld", param,
                                              cnt[0], cnt[1], cnt[2], cnt[3]);
                    else
                    {
                        char *c;
                        for (c = args; *c != '\0'; c++)
                            switch ( *c )
                            {
                            case '(':
								cnt[ROUND]++;
								break;
                            case '[':
								cnt[SQUARE]++;
								break;
                            case '{':
								cnt[CURLY]++;
								break;
                            case '<':
								cnt[ANGLE]++;
								break;

                            case ')':
								cnt[ROUND]--;
								break;
                            case ']':
								cnt[SQUARE]--;
								break;
                            case '}':
								cnt[CURLY]--;
								break;
                            case '>':
								cnt[ANGLE]--;
								break;
                            }
                        wlog(MSG, "(%lld) [%lld] {%lld} <%lld> : %s <- %s", 
                            cnt[ROUND], cnt[SQUARE], cnt[CURLY], cnt[ANGLE],
                            param, args);
                    }
                }
            }
        }
    }
}
