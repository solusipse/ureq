/*
https://github.com/solusipse/ureq

The MIT License (MIT)

Copyright (c) 2015 solusipse

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
                        SCROLL DOWN FOR ACTUAL EXAMPLE!
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <ws2tcpip.h>
#undef close
#define close(s) closesocket(s)
#undef write
#define write(s,b,n) send(s,b,n,0)
#undef read
#define read(s,b,n) recv(s,b,n,0)
#define bzero(p,n) memset(p,0,n)
#endif
#include <time.h>

#include "../ureq.h"

#define BUFSIZE 1024
#define QUEUE_SIZE 100
#define PORT 9999

void server(char *buffer, int socket);
int create_socket();
int main_b();
void error();
struct sockaddr_in set_address(struct sockaddr_in serveraddr);
void bind_to_port(int listen_socket, struct sockaddr_in serveraddr);


void get_client_address(struct sockaddr_in client_address) {
    struct hostent *hostp = NULL;
    char *hostaddrp = NULL;
    hostp = gethostbyaddr((const char *)&client_address.sin_addr.s_addr, sizeof(client_address.sin_addr.s_addr), AF_INET);
    if (hostp == NULL) error();
    hostaddrp = inet_ntoa(client_address.sin_addr);
    if (hostaddrp == NULL) error();
    printf("Client: %s (%s)\n", hostaddrp, hostp->h_name);
}

void perform_connection(int connection_socket, int listen_socket, struct sockaddr_in client_address, socklen_t address_lenght) {
    char buffer[BUFSIZE];
    connection_socket = accept(listen_socket, (struct sockaddr *) &client_address, &address_lenght);
    if (connection_socket < 0) error();
    bzero(buffer, BUFSIZE);
    read(connection_socket, buffer, BUFSIZE);
    server(buffer, connection_socket);
    close(connection_socket);
    //exit(0);
}

int main(int argc, char **argv) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,0), &wsaData);
#endif
    main_b();
    int listen_socket, connection_socket = 0, address_lenght, optval = 1;
    struct sockaddr_in server_address, client_address;
    listen_socket = create_socket();
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
    server_address = set_address(server_address);
    bind_to_port(listen_socket, server_address);
    address_lenght = sizeof(client_address);
    while (1) perform_connection(connection_socket, listen_socket, client_address, address_lenght);
}

int create_socket() {
    int lsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (lsocket < 0)
        error();
    return lsocket;
}

void error() {
    perror("ERROR");
    exit(1);
}

struct sockaddr_in set_address(struct sockaddr_in server_address) {
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((unsigned short)PORT);
    return server_address;
}

void bind_to_port(int listen_socket, struct sockaddr_in server_address) {
    if (bind(listen_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        error();
    if (listen(listen_socket, QUEUE_SIZE) < 0)
        error();
}


/*
                            ACTUAL EXAMPLE STARTS HERE
                        see example.c for more explainations
-------------------------------------------------------------------------------
Above that line is just simple and dirty tcp server.
Interesting part begins here.
*/

char *s_home(HttpRequest *r) {
    return "<h1>home</h1>";
}

char *s_test() {
    return "test";
}

char *s_param(HttpRequest *r) {
    /* This one shows how to handle GET parameters.
     * Please note, that ureq_get_param_value uses
     * common buffer for all operations, so store
     * copy data from it before calling it again */
    char *arg = NULL;

    strcpy(r->buffer, "data: ");
    arg = ureq_get_param_value(r, "data");
    strcat(r->buffer, arg);

    strcat(r->buffer, "<br>data2: ");
    arg = ureq_get_param_value(r, "data2");
    strcat(r->buffer, arg);

    return r->buffer;
}

char *s_post(HttpRequest *r) {
    if ( strcmp(UREQ_POST, r->type) != 0 )
        return  "Try requesting this page with POST method!<br>"
                "Feel free to use this form:<br>"
                "<form method='post'>"
                "Data: <input type='text' name='data'><br>"
                "Data2: <input type='text' name='data2'><br>"
                "<input type='submit' value='Submit'>"
                "</form>";

    char *arg = NULL;

    strcpy(r->buffer, "data: ");
    arg = ureq_post_param_value(r, "data");
    strcat(r->buffer, arg);

    strcat(r->buffer, "<br>data2: ");
    arg = ureq_post_param_value(r, "data2");
    strcat(r->buffer, arg);

    return r->buffer;
}

char *s_redirect_to_test(HttpRequest *r) {
    r->response.code = 302;
    r->response.header = "Location: /test";
    return "";
}

char *s_template(HttpRequest *r) {
    r->response.file = r->buffer;
    ureq_template(r, "first", "Yes");
    ureq_template(r, "second", "working");
    return "Does it work? {{first}}. Another keyword: {{second}}.";
}

char *s_exit() {
    // Using it only during development for memory leakage testing
    exit(1);
    return "";
}

int main_b() {
    /*
    That's a part of example main function. Declare urls to be served
    before receiving any requests.
    */

    ureq_serve("/", s_home, UREQ_GET);
    ureq_serve("/test", s_test, UREQ_GET);
    ureq_serve("/post", s_post, UREQ_ALL);
    ureq_serve("/param", s_param, UREQ_GET);
    ureq_serve("/redirect", s_redirect_to_test, UREQ_GET);
    ureq_serve("/template", s_template, UREQ_GET);

    ureq_serve("/exit", s_exit, UREQ_GET);

    return 0;
}

void server(char *buffer, int socket) {

    clock_t start = clock();
    HttpRequest req = ureq_init(buffer);

    while(ureq_run(&req)) {
        write(socket, req.response.data, req.len);
    }

    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;

    printf("Requested: %s (%s). Response: %d. It took: %f s.\n", \
            req.url, req.type, req.response.code, seconds);

    ureq_close(&req);
}
