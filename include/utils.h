#pragma once

#define BUFSIZE 65536
#define ERROR 42
#define LOG 44
#define FORBIDDEN 403
#define NOTFOUND 404
#define LOGFILE "basic-http-server.log"
#ifndef SIGCLD
#define SIGCLD SIGCHLD
#endif

typedef struct
{
    const char *ext;
    const char *filetype;
} mime_entry_t;

extern const mime_entry_t extensions[];

void writePortFile(const char *filename, int port);
void logger(int type, char *s1, char *s2, int socket_fd);
