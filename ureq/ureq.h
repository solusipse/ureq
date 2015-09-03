#ifndef UREQ_H
#define UREQ_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define GET "GET"
#define POST "POST"
#define ALL "ALL"

struct HttpRequest {
    char *type;
    char *url;
    char *version;
    char *data;
    char *params;
    char *response;
};

struct Page {
    char *url;
    char *(*func)();
    char *method;
};

struct Page *pages = NULL;
int pageCount = 0;


int ureq_parse_header(struct HttpRequest *req, char *r);

void ureq_get_header(char *h, char *r);
char *ureq_get_params(char *r);
void ureq_remove_parameters(char *b, char *u);
void ureq_get_parameters(char *b, char *u);

void ureq_send(char *r);
void ureq_run(struct HttpRequest *req, char *r);
void ureq_serve(char *url, char *(func)(char *), char *method );



#endif