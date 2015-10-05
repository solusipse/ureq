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


#ifdef ESP8266
// These are ESP8266 specific

    #include <mem.h>
    #include <osapi.h>

    #define realloc ureq_realloc
    #define malloc  ureq_malloc
    #define free    ureq_free

    #define printf(...) os_printf( __VA_ARGS__ )
    #define sprintf(...) os_sprintf( __VA_ARGS__ )

    char *ureq_malloc(size_t l) {
        return (char *) os_malloc(l);
    }

    void ureq_free(void *p) {
        if (p != NULL) {
            os_free(p);
            p = NULL;
        }
    }

#endif

// TODO: remove later
#define UREQ_USE_FILESYSTEM 1

#if defined UREQ_USE_FILESYSTEM && UREQ_USE_FILESYSTEM == 1
    #include "ureq_filesystem.c"
#endif


static int ureq_get_header(char *h, char *r) {
    char *p = NULL;
    strcpy(h, r);

    if ( strlen(r) <= 1 ) return 1;
    char *b = strtok_r(h, "\r\n", &p);
    if ( strlen(b) <= 1 ) return 1;

    strcpy(h, b);
    return 0;
}

int ureq_parse_header(HttpRequest *req, char *r) {

    char *header = malloc( strlen(r) + 1 );
    char *b = NULL;
    char *bk = NULL;

    if ( ureq_get_header(header, r) != 0 ) {
        free(header);
        return 1;
    }

    b = strtok_r(header, " ", &bk);
    if (( strncmp(GET, b, 3) != 0 )&&(strncmp(b, POST, 4) != 0 ))  {
        free(header);
        return 1;
    }
    req->type = malloc( strlen(b) + 1 );
    req->type[0] = '\0';
    strncat(req->type, b, strlen(b));

    b = strtok_r(NULL, " ", &bk);
    if (b == NULL) {
        free(header);
        return 1;
    }

    req->url = malloc( strlen(b) + 1 );
    req->url[0] = '\0';
    strncat(req->url, b, strlen(b));

    b = strtok_r(NULL, " ", &bk);
    if ( strncmp(b, "HTTP/1.", 7) != 0 ) {
        free(header);
        return 1;
    }
    req->version = malloc( strlen(b) + 1 );
    req->version[0] = '\0';
    strncat(req->version, b, strlen(b));

    b = strtok_r(NULL, " ", &bk);
    if (b != NULL) {
        free(header);
        return 1;
    }

    req->message = malloc( strlen(r) + 1 );
    req->message[0] = '\0';
    strncat(req->message, r, strlen(r));
    free(header);

    req->params             = NULL;
    req->response           = NULL;
    req->body               = NULL;
    req->responseHeaders    = NULL;
    req->mime               = NULL;

    req->responseCode       = 0;

    return 0;
}

void ureq_serve(char *url, char *(func)(HttpRequest *), char *method ) {
    struct Page page;
    page.url = url;
    page.func = func;
    page.method = method;

    #ifndef ESP8266
        pages = (struct Page *) realloc(pages, ++pageCount * sizeof(struct Page) );
        pages[pageCount-1] = page;
    #else
        pages[pageCount++] = page;
    #endif
    
}

HttpRequest ureq_init() {
    HttpRequest r;
    r.complete = -1;

    return r;
}

int ureq_run(HttpRequest *req, char *r ) {

    // TODO: on first run save everything what's needed to request struct

    if (req->complete == 1)
        return 0;

    if (req->complete == -1) {
        // If code equals to -1, it's the very first run,
        // parameters are set there and header is sent.
        // Data (if any), will be sent in next run(s).

        req->complete = -2;
        req->bigFile  =  0;
        req->len      =  0;

        int h = ureq_parse_header(req, r);
        // TODO: response with code 400 (Bad Request)
        if (h != 0) return -1;

        int i;
        for (i = 0; i < pageCount; i++) {
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
            if ( strcmp(ALL, pages[i].method) != 0 ) {
                // If there's no match between an url and method, skip
                // to next iteration.
                if ( strcmp(req->type, pages[i].method) != 0 ) {
                    continue;
                }
            }

            req->params = malloc( strlen(req->url) + 1 );
            req->params[0] = '\0';
            // Save get parameters to r->params
            ureq_get_query( req->params, req->url );

            // If method was POST, save body to r->message
            if ( strcmp (POST, req->type ) == 0 ) {
                req->body = malloc( strlen(req->message) + 1 );
                req->body = ureq_get_params(req->message);
            }
            // Get output generated by page
            //html = pages[i].func( req );

            // Run page function but don't save data from it
            // at first run (use it now only to set some things).
            pages[i].func( req );
            // Save pointer to page's func for later use
            req->func = pages[i].func;

            if (req->responseCode == 0)
                req->responseCode = 200;
            // Generate whole response
            //ureq_generate_response(req, html);
            // Return only header at first run
            req->response = ureq_generate_response_header(req);
            req->len = strlen(req->response);

            return req->complete;
        }
        #if defined UREQ_USE_FILESYSTEM && UREQ_USE_FILESYSTEM == 1
            UreqFile f = ureq_fs_open(req->url + 1);
            if (f.address == 0) {
                // File was not found
                req->responseCode = 404;
                ureq_generate_response(req, "404");
                return req->complete;
            } else {
                req->bigFile  =  1;
                req->complete = -2;

                if (req->responseCode == 0)
                    req->responseCode = 200;

                req->file = f;

                req->response = ureq_generate_response_header(req);
                req->len = strlen(req->response);

                return req->complete;
            }
        #else
            req->responseCode = 404;
            // TODO: move that below
            //       if code is 400 or 404 maybe send everything at once?
            ureq_generate_response(req, "404");
            return req->complete;
        #endif
    }
    
    if ((req->complete < -1) && (req->responseCode != 404)) {
        // TODO: check if req->response contains a header
        //       i.e.: if req->complete == -2
        if (req->complete == -2)
            free(req->response);

        req->complete--;

        if (req->bigFile) {
            #if defined UREQ_USE_FILESYSTEM && UREQ_USE_FILESYSTEM == 1
                // TODO: fix that
                if (req->file.size > 512) {
                    req->response = ureq_fs_read(req->file.address, 512, req->buffer);
                    req->file.address += 512;
                    req->file.size -= 512;
                    req->len = 512;
                    req->complete -= 1;
                } else {
                    req->len = req->file.size;
                    req->response = ureq_fs_read(req->file.address, req->file.size, req->buffer);
                    req->complete = 1;
                }
            #else
                // TODO: buffer read from func
                req->response = req->func(req);
                req->len = strlen(req->response);
            #endif
        } else {
            req->response = req->func(req);
            req->len = strlen(req->response);
            req->complete = 1;
        }
        return req->complete;
    }

    return 0;
}

static void ureq_generate_response(HttpRequest *r, char *html) {
    char *header = ureq_generate_response_header(r);
    r->response = malloc(strlen(header) + strlen(html) + 3);
    r->response[0] = '\0';

    strncat(r->response, header, strlen(header));
    strncat(r->response, html, strlen(html));
    strncat(r->response, "\r\n", 2);

    r->len = strlen(r->response);

    free(header);
}

static char *ureq_get_code_description(int c) {
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

static char *ureq_generate_response_header(HttpRequest *r) {
    char *ct = "Content-Type: ";
    // If user hasn't set any header parameters, set mime to default
    if (r->mime == NULL) {
        if ( r->responseCode == 200 || r->responseCode == 404 )
            r->mime = "text/html";

        if (r->responseHeaders != NULL) {
            char *bb = malloc( strlen(r->responseHeaders) + 1 );
            strcpy( bb, r->responseHeaders );
            r->responseHeaders = NULL;
            r->responseHeaders = malloc( strlen(bb) + 1 );
            strcpy( r->responseHeaders, bb );
            free(bb);
        }
    }

    if (r->mime != NULL) {
        char *br = malloc( strlen(r->mime) + strlen(ct) + 1 );
        strcpy( br, ct );
        strcat( br, r->mime );

        if (r->responseHeaders != NULL) {
            char *bb = malloc( strlen(r->responseHeaders) + 1 );
            strcpy(bb, r->responseHeaders );
            r->responseHeaders = NULL;
            r->responseHeaders = malloc( strlen(br) + strlen(bb) + 3 );
            strcpy( r->responseHeaders, br );
            strncat( r->responseHeaders, "\r\n", 2 );
            strncat( r->responseHeaders, bb, strlen(bb) );

            free(bb);
        } else {
            r->responseHeaders = malloc( strlen(br) + 3 /* \r\n */ + 1 );
            strcpy( r->responseHeaders, br );
        }
        strncat( r->responseHeaders, "\r\n", 2 );
        free(br);
    }

    if (r->responseHeaders == NULL)
        r->responseHeaders = "";

    char *desc = ureq_get_code_description(r->responseCode);

    size_t hlen = strlen(HTTP_V) + 4 /*response code*/ + strlen(desc) + \
                  strlen(r->responseHeaders) + 8/*spaces,specialchars*/;

    char *h = malloc( hlen + 1 );
    sprintf(h, "%s %d %s\r\n%s\r\n", HTTP_V, r->responseCode, desc, r->responseHeaders);
    
    return h;
}

static char *ureq_get_params(char *r) {
    char *data = malloc(strlen(r) + 1);
    char *out = malloc(strlen(r) + 1);
    strncat(data, r, strlen(r));

    char *bk;
    char *buf;
    for (buf = strtok_r(data,"\n", &bk); buf != NULL; buf = strtok_r(NULL, "\n", &bk)) {
        strcpy(out, buf);
    }
    free(data);

    return out;
}

char *ureq_get_param_value(char *r, char *arg) {
    char *data = malloc(strlen(r) + 1);
    char *out = malloc(strlen(r) + 1);
    strcpy(data, r);

    char *bk;
    char *buf;
    for (buf = strtok_r(data,"&", &bk); buf != NULL; buf = strtok_r(NULL, "&", &bk)) {

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

static void ureq_remove_parameters(char *b, char *u) {
    char *bk;
    strcpy(b, u);
    b = strtok_r(b, "?", &bk);
}

static void ureq_get_query(char *b, char *u) {
    if (strchr(u, '?') == NULL )
        return;
    char *bk;
    strncpy(b, u, strlen(u));
    b = strtok_r(b, "?", &bk);
    char *buf = strtok_r(NULL, "\n", &bk);
    strcpy(b, buf);
}

void ureq_close( HttpRequest *req ) {
    free(req->type);
    free(req->url);
    free(req->version);
    free(req->message);
    if (req->params != NULL)    free(req->params);
    //if (req->response != NULL)  free(req->response);
    if (req->body != NULL)      free(req->body);

    if (req->responseHeaders != NULL)
        if ( strlen(req->responseHeaders) > 0 )
            free(req->responseHeaders);

    req->responseCode = 0;
}

void ureq_finish() {
    #ifndef ESP8266
    free(pages);
    #endif
}

/*
TODO: add methods for:
    - minimal templating
    - loading files from page functions
    - setting custom mime-types (for bare files responses)
*/
