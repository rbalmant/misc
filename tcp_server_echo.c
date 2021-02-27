#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/unistd.h>

#define BUFLEN 512

int main(int argc, char **argv)
{
    int ss=-1, cs=-1;
    struct sockaddr_in *saddr, *caddr;
    socklen_t *addrlen;

    char buf[BUFLEN];
    int recvb, sentb;

    addrlen = malloc(sizeof(socklen_t));
    saddr = malloc(sizeof(struct sockaddr_in));
    caddr = malloc(sizeof(struct sockaddr_in));

    ss = socket(AF_INET, SOCK_STREAM, 0);
    if (ss < 0)
    {
        fprintf(stderr, "socket error\n");
        goto exit;
    }

    saddr->sin_addr.s_addr = INADDR_ANY;
    saddr->sin_port = htons(30000);
    saddr->sin_family = AF_INET;

    if (bind(ss, (struct sockaddr *)saddr, sizeof(struct sockaddr_in)))
    {
        fprintf(stderr, "bind error\n");
        goto exit;
    }

    if (listen(ss, 10))
    {
        fprintf(stderr, "listen error\n");
        goto exit;
    }

    cs = accept(ss, (struct sockaddr *) caddr, addrlen);
    if (cs < 0)
    {
        fprintf(stderr, "accept error\n");
        goto exit;
    }

    fprintf(stdout, "new client connected.\n");
    char cipaddr[128];

    inet_ntop(AF_INET, &(caddr->sin_addr.s_addr), cipaddr, INET_ADDRSTRLEN);

    while (1)
    {
        memset(buf, 0, BUFLEN);
        recvb = recv(cs, buf, BUFLEN, 0);
        if (recv > 0)
        {
            fprintf(stdout, "[%s] received %d bytes from client: %s", cipaddr, recvb, buf);
            sentb = send(cs, buf, recvb, 0);
            if (sentb)
            {
                fprintf(stdout, "[%s] echoed message  back to client.\n", cipaddr);
            }

            /*buf[recvb - 1] = '\0';
            if (!strcmp(buf, "quit") || !strcmp(buf, "exit"))
            {
                fprintf(stdout, "[%s] bye!\n", cipaddr);
                break;
            }*/
        }
    }

exit:
    close(ss);
    free(addrlen);
    free(saddr);
    free(caddr);

    return 0;
}