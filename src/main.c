#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils.h"

// TODO: Burde ha et opplegg for dynamisk portkonfigurasjon
#define PORT 8080

/* this is a child web server process, so we can exit on errors */
void web(char *rootDir, int fd, int hit)
{
    int j, file_fd, buflen;
    long i, ret, len;
    char *fstr;
    static char buffer[BUFSIZE + 1]; /* static so zero filled */

    ret = read(fd, buffer, BUFSIZE); /* read Web request in one go */
    if (ret == 0 || ret == -1)
    { /* read failure stop now */
        logger(FORBIDDEN, "failed to read browser request", "", fd);
    }
    if (ret > 0 && ret < BUFSIZE) /* return code is valid chars */
        buffer[ret] = 0;          /* terminate the buffer */
    else
        buffer[0] = 0;
    for (i = 0; i < ret; i++) /* remove CF and LF characters */
        if (buffer[i] == '\r' || buffer[i] == '\n')
            buffer[i] = '*';
    logger(LOG, "request", buffer, hit);
    if (strncmp(buffer, "GET ", 4) && strncmp(buffer, "get ", 4))
    {
        logger(FORBIDDEN, "Only simple GET operation supported", buffer, fd);
    }
    for (i = 4; i < BUFSIZE; i++)
    { /* null terminate after the second space to ignore extra stuff */
        if (buffer[i] == ' ')
        { /* string is "GET URL " +lots of other stuff */
            buffer[i] = 0;
            break;
        }
    }
    for (j = 0; j < i - 1; j++) /* check for illegal parent directory use .. */
        if (buffer[j] == '.' && buffer[j + 1] == '.')
        {
            logger(FORBIDDEN, "Parent directory (..) path names not supported", buffer, fd);
        }
    if (!strncmp(&buffer[0], "GET /\0", 6) || !strncmp(&buffer[0], "get /\0", 6)) /* convert no filename to index file */
        (void)strcpy(buffer, "GET /index.html");

    /* work out the file type and check we support it */
    buflen = strlen(buffer);
    fstr = (char *)0;
    for (i = 0; extensions[i].ext != 0; i++)
    {
        len = strlen(extensions[i].ext);
        if (!strncmp(&buffer[buflen - len], extensions[i].ext, len))
        {
            fstr = extensions[i].filetype;
            break;
        }
    }
    if (fstr == 0)
        logger(FORBIDDEN, "file extension type not supported", buffer, fd);

    char *filePath = malloc(strlen(rootDir) + strlen(&buffer[5]) + 2);
    sprintf(filePath, "%s/%s", rootDir, &buffer[5]);
    if ((file_fd = open(filePath, O_RDONLY)) == -1)
    { /* open the file for reading */
        logger(NOTFOUND, "failed to open file", &buffer[5], fd);
    }
    logger(LOG, "SEND", &buffer[5], hit);
    len = (long)lseek(file_fd, (off_t)0, SEEK_END);                                                                                                       /* lseek to the file end to find the length */
    (void)lseek(file_fd, (off_t)0, SEEK_SET);                                                                                                             /* lseek back to the file start ready for reading */
    (void)sprintf(buffer, "HTTP/1.1 200 OK\nServer: basic-http-server/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n", 1, len, fstr); /* Header + a blank line */
    logger(LOG, "Header", buffer, hit);
    (void)write(fd, buffer, strlen(buffer));

    /* send file in 8KB block - last block may be smaller */
    while ((ret = read(file_fd, buffer, BUFSIZE)) > 0)
    {
        (void)write(fd, buffer, ret);
    }
    sleep(1); /* allow socket to drain before signalling the socket is closed */
    close(fd);
    exit(1);
}

int startHttpServer(char *directory, int port)
{
    printf("Starting HTTP server on port %d...\n", port);

    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    int socketfd, listenfd, pid, hit;
    struct sockaddr_in cli_addr;
    bzero(&cli_addr, sizeof(cli_addr));
    socklen_t length;

    logger(LOG, "basic-http-server starting", directory, getpid());

    /* setup the network socket */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        logger(ERROR, "system call", "socket", 0);
    port = PORT;
    if (port < 0 || port > 60000)
        // TODO: Cast port to string properly
        logger(ERROR, "Invalid port number (try 1->60000)", (char *)port, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        logger(ERROR, "system call", "bind", 0);
    if (listen(server_fd, 64) < 0)
        logger(ERROR, "system call", "listen", 0);
    for (hit = 1;; hit++)
    {
        length = sizeof(cli_addr);
        if ((socketfd = accept(server_fd, (struct sockaddr *)&cli_addr, &length)) < 0)
            logger(ERROR, "system call", "accept", 0);
        if ((pid = fork()) < 0)
        {
            logger(ERROR, "system call", "fork", 0);
        }
        else
        {
            if (pid == 0)
            { /* child */
                (void)close(listenfd);
                web(directory, socketfd, hit); /* never returns */
            }
            else
            { /* parent */
                (void)close(socketfd);
            }
        }
    }
}

int main(void)
{
    printf("Hello, basic HTTP server!\n");

    writePortFile("server.port", PORT);

    startHttpServer("static", PORT);

    return 0;
}
