#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ureq/ureq.c"

#define BUFSIZE 1024
#define QUEUE_SIZE 100
#define PORT 9999

void server(char *buffer, int socket);
int create_socket();
void error();
struct sockaddr_in set_address(struct sockaddr_in serveraddr);
void bind_to_port(int listen_socket, struct sockaddr_in serveraddr);

void get_client_address(struct sockaddr_in client_address)
{
    struct hostent *hostp;
    char *hostaddrp;

    hostp = gethostbyaddr((const char *)&client_address.sin_addr.s_addr, sizeof(client_address.sin_addr.s_addr), AF_INET);
    if (hostp == NULL) error();

    hostaddrp = inet_ntoa(client_address.sin_addr);
    if (hostaddrp == NULL) error();

    printf("Client: %s (%s)\n", hostaddrp, hostp->h_name);
}



void perform_connection(int connection_socket, int listen_socket, struct sockaddr_in client_address, unsigned int address_lenght)
{
    char buffer[BUFSIZE];

    connection_socket = accept(listen_socket, (struct sockaddr *) &client_address, &address_lenght);
    if (connection_socket < 0) error();

    bzero(buffer, BUFSIZE);

    read(connection_socket, buffer, BUFSIZE);

    server(buffer, connection_socket);

    close(connection_socket);

}

int main(int argc, char **argv) {
    int listen_socket, connection_socket = 0, address_lenght, optval = 1;
    struct sockaddr_in server_address, client_address;

    listen_socket = create_socket();
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    server_address = set_address(server_address);
    bind_to_port(listen_socket, server_address);
    address_lenght = sizeof(client_address);

    while (1) perform_connection(connection_socket, listen_socket, client_address, address_lenght);
}

int create_socket()
{
    int lsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (lsocket < 0)
        error();
    return lsocket;
}

void error()
{
    perror("ERROR");
    exit(1);
}

struct sockaddr_in set_address(struct sockaddr_in server_address)
{
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((unsigned short)PORT);
    return server_address;
}

void bind_to_port(int listen_socket, struct sockaddr_in server_address)
{
    if (bind(listen_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) 
        error();
    if (listen(listen_socket, QUEUE_SIZE) < 0)
        error();
}

/* EXAMPLE STARTS HERE, NOT IMPORTANT ABOVE */

char *s_home(char *request) {
    printf("%s\n", request);
    return "home";
}

void server(char *buffer, int socket) {
    //printf("Received: %s\n", buffer);

    struct HttpRequest req;
    if ( ureq_parse_header(buffer, &req) != 0 )
        return;

    ureq_serve("/", s_home, GET);

    ureq_run(&req);

    write(socket, "test", strlen("test"));
}