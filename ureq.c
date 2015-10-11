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

#ifdef UREQ_ESP8266
    #define UREQ_STATIC_LIST
#endif

#include "ureq.h"

#ifdef UREQ_ESP8266
    #include "hardware/ureq_esp8266.h"
#endif

#ifdef UREQ_USE_FILESYSTEM
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

static int ureq_check_method_validity(char *m) {
    int i;
    for(i = 0; UreqMethods[i] != NULL; i++)
        if (strcmp(UreqMethods[i], m) == 0)
            return 1;
    return 0;
}

static int ureq_parse_header(HttpRequest *req, char *r) {

    char *header = malloc( strlen(r) + 1 );
    char *b = NULL;
    char *bk = NULL;

    if ( ureq_get_header(header, r) != 0 ) {
        free(header);
        return 1;
    }

    b = strtok_r(header, " ", &bk);
    if (ureq_check_method_validity(b) == 0) {
        free(header);
        return 1;
    }
    req->type = malloc( strlen(b) + 1 );
    strcpy(req->type, b);

    b = strtok_r(NULL, " ", &bk);
    if (b == NULL) {
        free(header);
        return 1;
    }

    req->url = malloc( strlen(b) + 1 );
    strcpy(req->url, b);

    b = strtok_r(NULL, " ", &bk);
    if (b == NULL) {
        free(header);
        return 1;
    }
    if ( strncmp(b, "HTTP/1.", 7) != 0 ) {
        free(header);
        return 1;
    }
    req->version = malloc( strlen(b) + 1 );
    strcpy(req->version, b);

    b = strtok_r(NULL, " ", &bk);
    if (b != NULL) {
        free(header);
        return 1;
    }

    req->message = malloc( strlen(r) + 1 );
    strcpy(req->message, r);
    free(header);

    req->params             = NULL;
    req->response.data      = NULL;
    req->body               = NULL;
    req->response.header    = NULL;
    req->response.mime      = NULL;
    req->response.code      = 0;

    return 0;
}

void ureq_serve(char *url, char *(func)(HttpRequest *), char *method ) {
    struct Page page;
    page.url = url;
    page.func = func;
    page.method = method;

    #ifndef UREQ_STATIC_LIST
        pages = (struct Page *) realloc(pages, ++pageCount * sizeof(struct Page) );
        pages[pageCount-1] = page;
    #else
        pages[pageCount++] = page;
    #endif
    
}

HttpRequest ureq_init(char *ur) {
    HttpRequest r;

    r.complete = -1;
    r.valid    =  0;
    r.bigFile  =  0;
    r.len      =  0;

    r.type       = NULL;
    r.url        = NULL;
    r.version    = NULL;
    r.message    = NULL;
    r.params     = NULL;
    r.body       = NULL;
    r.page404    = NULL;

    if ( strlen(ur) > MAX_REQUEST_SIZE ) {
        r.response.code  = 413;
        r.valid = 0;
        return r;
    }

    int h = ureq_parse_header(&r, ur);
    if (h != 0) r.valid = 0;
    else r.valid = 1;

    return r;
}

static int ureq_first_run(HttpRequest *req) {
    req->complete = -2;

    int i;
    for (i = 0; i < pageCount; i++) {
        /*
        This loop iterates through a pages list and compares
        urls and methods to requested ones. If there's a match,
        it calls a corresponding function and saves http
        response to req.response.
        */

        // Start from checking if page's special (e.g. 404)
        if ( !req->page404 )
            if ( strcmp(pages[i].url, "404") == 0)
                req->page404 = pages[i].func;

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

        // Save get parameters to r->params
        ureq_get_query( req );

        // If method was POST, save body to r->message
        if ( strcmp (POST, req->type ) == 0 ) {
            req->body = malloc( strlen(req->message) + 1 );
            req->body = ureq_get_params(req->message);
        }
        // Run page function but don't save data from it
        // at first run (use it now only to set some things).
        // If user returns blank string, don't send data again
        // (complete code 2)
        if (strlen(pages[i].func( req )) == 0)
            req->complete = 2;
        // Save pointer to page's func for later use
        req->func = pages[i].func;

        if (req->response.code == 0)
            req->response.code = 200;

        // Return only header at first run
        req->response.data = ureq_generate_response_header(req);
        req->len = strlen(req->response.data);

        return req->complete;
    }
    #if defined UREQ_USE_FILESYSTEM
        return ureq_fs_first_run(req);
    #else
        return ureq_set_404_response(req);
    #endif
}

static int ureq_next_run(HttpRequest *req) {
    if (req->complete == -2) {
        free(req->response.data);
    }

    struct Response respcpy = req->response;
    req->complete--;

    if (req->bigFile) {
        #if defined UREQ_USE_FILESYSTEM
            if (req->file.size > 1024) {
                respcpy.data = ureq_fs_read(req->file.address, 1024, req->buffer);
                req->file.address += 1024;
                req->file.size -= 1024;
                req->len = 1024;
                req->complete -= 1;
            } else {
                req->len = req->file.size;
                respcpy.data = ureq_fs_read(req->file.address, req->file.size, req->buffer);
                req->complete = 1;
            }
        #else
            // TODO: buffer read from func
            respcpy.data = req->func(req);
            req->len = strlen(respcpy.data);
        #endif
    } else {
        respcpy.data = req->func(req);
        req->len = strlen(respcpy.data);
        req->complete = 1;
    }
    req->response = respcpy;
    return req->complete;

}

static int ureq_set_404_response(HttpRequest *r) {
    char *page;
    r->response.code = 404;

    if (r->page404)
        page = r->page404(r);
    else
        page = (char *) UREQ_HTML_PAGE_404;
    
    ureq_generate_response(r, page);
    return r->complete = 1;
}

static char *ureq_get_error_page(HttpRequest *r) {
    /* TODO:
     * This method probably causes leaking, check that
     */
    char *desc = ureq_get_code_description(r->response.code);
    sprintf(r->buffer, "%s%d %s%s%d %s%s", \
            UREQ_HTML_HEADER, r->response.code, desc, \
            UREQ_HTML_BODY, r->response.code, desc, \
            UREQ_HTML_FOOTER);
    return r->buffer;
}

static int ureq_set_error_response(HttpRequest *r) {
    if (!r->response.code) r->response.code = 400;
    r->response.mime = "text/html";
    r->response.header = "";
    ureq_generate_response(r, ureq_get_error_page(r));
    return r->complete = 1;
}

int ureq_run(HttpRequest *req) {
    /* Code meanings:
     * 1:   everything went smooth
     * 2:   user provided blank string, don't send data
     *      for the second time (and free header)
     * <-1: still running, on -2 free header which is
     *      dynamically alloced */
    if (req->complete == 1)
        return 0;

    if (req->complete == 2) {
        free(req->response.data);
        return 0;
    }

    if (req->complete == -1) {
        // If code equals to -1, it's the very first run,
        // parameters are set there and header is sent.
        // Data (if any), will be sent in next run(s).

        // If request was invalid, set everything to null
        if (!req->valid) return ureq_set_error_response(req);

        return ureq_first_run(req);
    }
    
    if ((req->complete < -1) && (req->response.code != 404)) {
        return ureq_next_run(req);
    }

    return 0;
}

static void ureq_generate_response(HttpRequest *r, char *html) {
    char *header = ureq_generate_response_header(r);
    r->response.data = malloc(strlen(header) + strlen(html) + 3);

    strcpy(r->response.data, header);
    strcat(r->response.data, html);
    strncat(r->response.data, "\r\n", 2);

    r->len = strlen(r->response.data);

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
    if (c == 413) return "Request-URI Too Long";
    if (c == 500) return "Internal Error";
    if (c == 503) return "Service Temporarily Overloaded";

    return "Not Implemented";
}

static char *ureq_set_mimetype(HttpRequest *r) {
    const char *e = strrchr(r->url, '.');
    if (e) e += 1;
    else return "text/html";

    int i = 0;
    for (i=0; UreqMimeTypes[i].ext != NULL; i++)
        if (strcmp(UreqMimeTypes[i].ext, e) == 0) break;

    return (char*) UreqMimeTypes[i].mime;
}

static char *ureq_generate_response_header(HttpRequest *r) {
    char *ct = "";
    // Set default mime type if blank
    if (r->response.mime == NULL) {
        if ( r->response.code == 200 || r->response.code == 404 ) {
            r->response.mime = ureq_set_mimetype(r);
            ct = "Content-Type: ";
        } else {
            r->response.mime = "";
        }
    } else {
        ct = "Content-Type: ";
    }

    char *br = malloc( strlen(r->response.mime) + strlen(ct) + 1 );
    strcpy( br, ct );
    strcat( br, r->response.mime );

    if (r->response.header != NULL) {
        char *bb = malloc( strlen(r->response.header) + 1 );
        strcpy(bb, r->response.header );
        r->response.header = malloc( strlen(br) + strlen(bb) + strlen("\r\n") + 1 );
        strcpy( r->response.header, br );

        if (strlen(br)>0)
            strcat( r->response.header, "\r\n" );

        strcat( r->response.header, bb );

        free(bb);
    } else {
        r->response.header = malloc( strlen(br) + strlen("\r\n") + 1 );
        strcpy( r->response.header, br );
    }
    strcat(r->response.header, "\r\n");
    free(br);

    if (r->response.header == NULL)
        r->response.header = "";

    char *desc = ureq_get_code_description(r->response.code);

    size_t hlen = strlen(HTTP_V) + 4 /*response code*/ + strlen(desc) + \
                  strlen(r->response.header) + 8/*spaces,specialchars*/;

    char *h = malloc( hlen + 1 );
    sprintf(h, "%s %d %s\r\n%s\r\n", HTTP_V, r->response.code, desc, r->response.header);
    
    return h;
}

static char *ureq_get_params(char *r) {
    char *data = malloc(strlen(r) + 1);
    char *out = malloc(strlen(r) + 1);
    strcpy(data, r);

    char *bk;
    char *buf;
    for (buf = strtok_r(data,"\n", &bk); buf != NULL; buf = strtok_r(NULL, "\n", &bk)) {
        strcpy(out, buf);
    }
    free(data);

    return out;
}

char *ureq_get_param_value(HttpRequest *r, char *arg) {
    char *data = malloc(strlen(r->params) + 1);
    strcpy(data, r->params);

    char *bk, *buf;
    for (buf = strtok_r(data,"&", &bk); buf != NULL; buf = strtok_r(NULL, "&", &bk)) {

        if (strstr(buf, arg) == NULL) {
            r->_buffer[0] = '\0';
            continue;
        }

        char *sptr = NULL;
        buf = strtok_r(buf, "=", &sptr);

        if ( strcmp(buf, arg) == 0 ) {
            strcpy(r->_buffer, sptr);
            break;
        }
        else {
            r->_buffer[0] = '\0';
        }

    }
    free(data);

    return r->_buffer;
}

static void ureq_remove_parameters(char *b, char *u) {
    char *bk;
    strcpy(b, u);
    b = strtok_r(b, "?", &bk);
}

static void ureq_get_query(HttpRequest *r) {
    char *q = strchr(r->url, '?');
    if (q == NULL) return;

    r->params = r->url + (int)(q - r->url) + 1;
}

void ureq_close( HttpRequest *req ) {
    if (req->type)      free(req->type);
    if (req->url)       free(req->url);
    if (req->version)   free(req->version);
    if (req->message)   free(req->message);
    if (req->body)      free(req->body);

    if (!req->valid || req->response.code == 404)
        free(req->response.data);

    if (req->response.header != NULL)
        if ( strlen(req->response.header) > 1 ) { 
            free(req->response.header);
        }
}

void ureq_finish() {
    #ifndef UREQ_STATIC_LIST
    free(pages);
    #endif
}

/*
TODO: add methods for:
    - minimal templating
    - loading files from page functions
    - setting custom mime-types (for bare files responses)
*/
