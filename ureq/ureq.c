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

#include "ureq.h"

void ureq_get_header(char *h, char *r) {
    char *p = NULL;
    strncpy(h, r, strlen(r));
    char *b = strtok_r(h, "\n", &p);
    strncpy(h, b, strlen(b));
}

int ureq_parse_header(struct HttpRequest *req, char *r) {

    char *header = malloc( strlen(r) + 1 );
    header[0] = '\0';
    char *b = NULL;

    // TODO: wrk causes segfault here, need to check that
    ureq_get_header(header, r);
    b = strtok(header, " ");
    if (( strncmp(GET, b, 3) != 0 )&&(strncmp(b, POST, 4) != 0 ))  {
        free(header);
        return 1;
    }
    req->type = malloc( strlen(b) + 1 );
    req->type[0] = '\0';
    strncat(req->type, b, strlen(b));

    b = strtok(NULL, " ");
    if (b == NULL) {
        return 1;
    }

    req->url = malloc( strlen(b) + 1 );
    req->url[0] = '\0';
    strncat(req->url, b, strlen(b));

    b = strtok(NULL, " ");
    if ( strncmp(b, "HTTP/1.", 7) != 0 ) {
        return 1;
    }
    req->version = malloc( strlen(b) + 1 );
    req->version[0] = '\0';
    strncat(req->version, b, strlen(b));

    b = strtok(NULL, " ");
    if (b != NULL) {
        return 1;
    }

    req->message = malloc( strlen(r) + 1 );
    req->message[0] = '\0';
    strncat(req->message, r, strlen(r));

    req->params = NULL;
    req->response = NULL;
    req->body = NULL;
    req->responseCode = 0;
    free(header);

    return 0;
}

void ureq_serve(char *url, char *(func)(HttpRequest *), char *method ) {
    struct Page page;
    page.url = url;
    page.func = func;
    page.method = method;

    pages = (struct Page *) realloc(pages, ++pageCount * sizeof(struct Page) );
    pages[pageCount-1] = page;
}

int ureq_run( struct HttpRequest *req, char *r ) {
    
    int h = ureq_parse_header(req, r);
    // TODO: response with code 400 (Bad Request)
    if (h != 0) return -1;

    for (int i = 0; i < pageCount; i++) {
        /*
        This loop iterates through a pages list and compares
        urls and methods to requested ones. If there's a match,
        it calls a corresponding function and saves http
        response to req.response.
        */
        char *plain_url = malloc( strlen(req->url) + 1 );

        ureq_remove_parameters(plain_url, req->url);

        // If there's no match between this, skip to next iteration.
        if ( strcmp(plain_url, pages[i].url) != 0 ) {
            free(plain_url);
            continue;
        }

        free(plain_url);

        // If request type is ALL, corresponding function is always called,
        // no matter which method type was used.
        if ( strcmp(ALL, pages[i].method) != 0 )
            // If there's no match between an url and method, skip
            // to next iteration.
            if ( strcmp(req->type, pages[i].method) != 0 ) {
                continue;
            }

        char *html = NULL;

        req->params = malloc( strlen(req->url) + 1 );
        req->params[0] = '\0';

        ureq_get_query( req->params, req->url );

        if ( strcmp (POST, req->type ) == 0 ) {
            req->body = malloc( strlen(req->message) + 1 );
            req->body = ureq_get_params(req->message);
        }

        html = pages[i].func( req );

        if (req->responseCode == 0)
            req->responseCode = 200;

        char *header = ureq_generate_response_header(req);

        char *buf = malloc(strlen(header) + strlen(html) + 1);
        buf[0] = '\0';
        
        strncat(buf, header, strlen(header));
        strncat(buf, html, strlen(html));

        req->response = malloc( strlen(buf) + 1 );
        req->response[0] = '\0';

        strncat(req->response, buf, strlen(buf));
        free(buf);

        // TODO: return code from request struct
        return req->responseCode;

    }
    req->response = malloc( 4 );
    req->response[0] = '\0';
    strncat(req->response, "404", 3);
    return 404;
    // TODO: if there is no requested url, check for file in filesystem
    //       (future feature)
}

char *ureq_get_code_description(int c) {
    if (c == 200) return "OK";
    if (c == 302) return "Found";
    if (c == 400) return "Bad Request";
    if (c == 401) return "Unauthorized";
    if (c == 403) return "Forbidden";
    if (c == 404) return "Not Found";
    if (c == 408) return "Request Timeout";
    if (c == 500) return "Internal Error";
    if (c == 503) return "Service Temporarily Overloaded";

    return "Not Implemented";
}

char *ureq_generate_response_header(HttpRequest *r) {
    // TODO: make this dynamic
    // TODO: use different content types
    // TODO: map request code to request description
    // TODO: don't do above if one of these were already set
    static char h[128] = "HTTP/1.1";
    char c[6] = "\0";
    snprintf(c, 6, " %d ", r->responseCode);
    strncat(h, c, strlen(c));
    strncat(h, "OK\nContent-Type: text/html\n\n", 30);
    return h;
}

char *ureq_get_params(char *r) {
    char *data = malloc(strlen(r) + 1);
    char *out = malloc(strlen(r) + 1);
    strncat(data, r, strlen(r));

    for (char *buf = strtok(data,"\n"); buf != NULL; buf = strtok(NULL, "\n")) {
        strcpy(out, buf);
    }
    free(data);

    return out;
}

char *ureq_get_param_value(char *r, char *arg) {
    char *data = malloc(strlen(r) + 1);
    char *out = malloc(strlen(r) + 1);
    strcpy(data, r);

    for (char *buf = strtok(data,"&"); buf != NULL; buf = strtok(NULL, "&")) {

        if (strstr(buf, arg) == NULL)
            continue;

        char *sptr = NULL;

        buf = strtok_r(buf, "=", &sptr);

        if ( strcmp(buf, arg) == 0 ) {
            strcpy(out, sptr);
        }
        else {
            out[0] = '\0';
        }
    }
    free(data);

    return out;
}

void ureq_remove_parameters(char *b, char *u) {
    strcpy(b, u);
    b = strtok(b, "?");
}

void ureq_get_query(char *b, char *u) {
    if (strchr(u, '?') == NULL )
        return;
    strncpy(b, u, strlen(u));
    b = strtok(b, "?");
    char *buf = strtok(NULL, "\n");
    strcpy(b, buf);
}

void ureq_close( struct HttpRequest *req ) {
    free(req->type);
    free(req->url);
    free(req->version);
    free(req->message);
    if (req->params != NULL)
        free(req->params);
    if (req->response != NULL)
        free(req->response);
    if (req->body != NULL)
        free(req->body);
    req->responseCode = 0;
}

void ureq_finish() {
    free(pages);
}