#ifndef UREQ_H
#define UREQ_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define GET "GET"
#define POST "POST"
#define ALL "ALL"

#define BUFSIZE 128

struct HttpRequest {
    char *type;
    char *url;
    char *version;
    char *data;
    char *params;
};

struct Page {
    char *url;
    char *(*func)();
    char *method;
};

struct Page *pages = NULL;
int pageCount = 0;


int ureq_parse_header(char *r, struct HttpRequest *req);

void ureq_get_header(char *h, char *r);
char *ureq_get_post_arguments(char *r);
void ureq_remove_parameters(char *b, char *u);
void ureq_get_parameters(char *b, char *u);

void ureq_send(char *r);
void ureq_run(struct HttpRequest *req);
void ureq_serve(char *url, char *(func)(char *), char *method );



#endif