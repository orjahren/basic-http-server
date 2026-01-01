#include "utils.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

const mime_entry_t extensions[] = {
    {"gif", "image/gif"}, {"jpg", "image/jpg"}, {"jpeg", "image/jpeg"}, {"png", "image/png"}, {"ico", "image/ico"}, {"zip", "image/zip"}, {"gz", "image/gz"}, {"tar", "image/tar"}, {"htm", "text/html"}, {"html", "text/html"}, {0, 0}};

void writePortFile(const char *filename, int port)
{

    printf("Writing port %d to file %s\n", port, filename);
    FILE *file = fopen(filename, "w");
    if (file != NULL)
    {
        fprintf(file, "%d\n", port);
        fclose(file);
    }
    else
    {
        printf("Failed to open file %s for writing\n", filename);
    }
}

void logger(int type, char *s1, char *s2, int socket_fd)
{
    int fd;
    char logbuffer[BUFSIZE * 2];

    switch (type)
    {
    case ERROR:
        (void)sprintf(logbuffer, "ERROR: %s:%s Errno=%d exiting pid=%d", s1, s2, errno, getpid());
        break;
    case FORBIDDEN:
        (void)write(socket_fd, "HTTP/1.1 403 Forbidden\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>403 Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n", 271);
        (void)sprintf(logbuffer, "FORBIDDEN: %s:%s", s1, s2);
        break;
    case NOTFOUND:
        (void)write(socket_fd, "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\nThe requested URL was not found on this server.\n</body></html>\n", 224);
        (void)sprintf(logbuffer, "NOT FOUND: %s:%s", s1, s2);
        break;
    case LOG:
        (void)sprintf(logbuffer, " INFO: %s:%s:%d", s1, s2, socket_fd);
        break;
    }
    /* No checks here, nothing can be done with a failure anyway */
    if ((fd = open(LOGFILE, O_CREAT | O_WRONLY | O_APPEND, 0644)) >= 0)
    {
        (void)write(fd, logbuffer, strlen(logbuffer));
        (void)write(fd, "\n", 1);
        (void)close(fd);
    }
    if (type == ERROR || type == NOTFOUND || type == FORBIDDEN)
        exit(42);
}
