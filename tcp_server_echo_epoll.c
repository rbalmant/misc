#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 512

#define ADDRESS "0.0.0.0"
#define PORT 30000
#define BACKLOG 128
#define MAXEVENT 10
#define TIMEOUT 100 * 60 * 5 // 5 minutes

int main(int argc, char* argv[])
{
    int epoll;
    struct epoll_event listen_sock_ev, temp_ev, *ev;

    int listening_socket;

    int res, i;

    struct sockaddr_in *listening_socket_addr;

    char buf[BUFSIZE];

    epoll = epoll_create1(0);
    if (epoll == -1)
    {
        fprintf(stderr, "could not create epoll instance. error: %s\n", strerror(errno));
        return -1;
    }

    listening_socket_addr = malloc(sizeof(struct sockaddr_in));
    if (listening_socket_addr == NULL)
    {
        fprintf(stderr, "(malloc) error: %s\n", strerror(errno));
        return -1;
    }

    memset(listening_socket_addr, 0, sizeof(struct sockaddr_in));

    // IPv4
    listening_socket_addr->sin_port = htons(PORT);
    listening_socket_addr->sin_family = AF_INET;
    listening_socket_addr->sin_addr.s_addr = inet_addr(ADDRESS);

    listening_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listening_socket < 0)
    {
        fprintf(stderr, "(socket) error: %s\n", strerror(errno));
        free(listening_socket_addr);
        return -1;
    }

    int listening_socket_reuseaddr = 1;

    res = setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &listening_socket_reuseaddr, sizeof(listening_socket_reuseaddr));
    if (res < 0)
    {
        fprintf(stderr, "(setsockopt) error: %s\n", strerror(errno));
        free(listening_socket_addr);
        return -1;
    }

    res = bind(listening_socket, (struct sockaddr *)listening_socket_addr, sizeof(struct sockaddr_in));
    if (res < 0)
    {
        fprintf(stderr, "(bind) error: %s\n", strerror(errno));
        close(listening_socket);
        free(listening_socket_addr);
    }

    res = listen(listening_socket, BACKLOG);
    if (res < 0)
    {
        fprintf(stderr, "(listen) error: %s\n", strerror(errno));
        close(listening_socket);
        free(listening_socket_addr);
    }

    listen_sock_ev.events = EPOLLIN;
    listen_sock_ev.data.fd = listening_socket;

    res = epoll_ctl(epoll, EPOLL_CTL_ADD, listening_socket, &listen_sock_ev);
    if (res < 0)
    {
        fprintf(stderr, "(epoll_ctl) error: %s\n", strerror(errno));
        close(listening_socket);
        free(listening_socket_addr);
    }

    ev = malloc(sizeof(struct epoll_event) * MAXEVENT);

    while((res = epoll_wait(epoll, ev, MAXEVENT, TIMEOUT)) > 0) // breaks on timeout (0)
    {
        i=0;
        for (; i<res; i++)
        {
            if ((ev + i)->data.fd == listening_socket)
            {
                fprintf(stdout, "attempting to accept connection...\n");
                // We must accept new connections.
                memset(&temp_ev, 0, sizeof(struct epoll_event));

                temp_ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET; // Notifies when there is something to read/write/disconnect.

                int conn_socket = accept(listening_socket, NULL, NULL); // Don't care about address.
                if (conn_socket < 0)
                {
                    fprintf(stderr, "(accept) error: %s\n", strerror(errno));
                    continue;
                }

                int flags;

                flags = fcntl(conn_socket, F_GETFD);
                if (flags < 0)
                {
                    fprintf(stderr, "(fcntl) error: %s\n", strerror(errno));
                    continue;
                }

                flags |= O_NONBLOCK;
                res = fcntl(conn_socket, F_SETFD, flags);
                if (flags < 0)
                {
                    fprintf(stderr, "(fcntl) error: %s\n", strerror(errno));
                    continue;
                }

                temp_ev.data.fd = conn_socket;

                res = epoll_ctl(epoll, EPOLL_CTL_ADD, conn_socket, &temp_ev);
                if (res < 0)
                {
                    fprintf(stderr, "(epoll_ctl) error: %s\n", strerror(errno));
                    continue;
                }

                fprintf(stdout, "New connection.\n");
                continue;
            }
            
            memset(buf, 0, BUFSIZE);

            if (((ev + i)->events & EPOLLIN))
            {
                // TODO: discover why the following read call may never return
                //res = read((ev + i)->data.fd, buf, BUFSIZE);
                res = recv((ev + i)->data.fd, buf, BUFSIZE, 0);
                if (res < 0)
                {
                    if (res == EAGAIN || res == EAGAIN)
                    {
                        continue;
                    }

                    fprintf(stderr, "(read) error: %s\n", strerror(errno));
                    continue;
                }

                if ((ev + i)->events & EPOLLOUT)
                {
                    //res = write((ev + i)->data.fd, buf, res);
                    res = send((ev + i)->data.fd, buf, res, 0);
                    if (res < 0)
                    {
                        if (res == EAGAIN || res == EAGAIN)
                        {
                            continue;
                        }

                        fprintf(stderr, "(send) error: %s\n", strerror(errno));
                        continue;
                    }
                }
            }

            if ((ev + i)->events & EPOLLRDHUP || (ev + i)->events & EPOLLHUP || (ev + i)->events & EPOLLERR)
            {
                res = epoll_ctl(epoll, EPOLL_CTL_DEL, (ev + i)->data.fd, NULL);
                if (res < 0)
                {
                    fprintf(stderr, "(epoll_ctl) error: %s\n", strerror(errno));
                    continue;
                }
            }
        }
    }


    free(listening_socket_addr);
    close(listening_socket);

    return 0;
}
