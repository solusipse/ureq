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

#ifndef UREQ_H
#define UREQ_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define GET 	"GET"
#define POST 	"POST"
#define ALL 	"ALL"
#define PUT 	"PUT"
#define DELETE	"DELETE"
#define HTTP_V  "HTTP/1.1"

typedef struct HttpRequest {
    char *type;
    char *url;
    char *version;
    char *message;
    char *params;
    char *body;

    char *response;

    int  responseCode;
    char *responseDescription;
    char *responseHeaders;
    char *mime;

    int complete;
    int bigFile;

    char *(*func)(struct HttpRequest *);
} HttpRequest;

struct Page {
    char *url;
    char *(*func)();
    char *method;
};

#ifndef ESP8266

    static struct Page *pages = NULL;
    static int pageCount = 0;

#else

    static struct Page pages[16];
    static int pageCount = 0;

#endif



void ureq_serve(char *url, char *(func)(HttpRequest *), char *method );

HttpRequest ureq_init();

void ureq_close( struct HttpRequest *req );
void ureq_finish();

int ureq_run(struct HttpRequest *req, char *r);
int ureq_parse_header(struct HttpRequest *req, char *r);


static int  ureq_get_header(char *h, char *r);
static void ureq_remove_parameters(char *b, char *u);
static void ureq_get_query(char *b, char *u);
static void ureq_generate_response(HttpRequest *r, char *html);

static char *ureq_get_params(char *r);
static char *ureq_generate_response_header(HttpRequest *r);

#endif
