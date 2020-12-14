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

#include "../ureq.h"

/* --------------------------------v PAGES v-------------------------------- */

/*
These functions are called when corresponding request is issued by a client.
Connect these functions with methods using ureq_serve function:
e.g.: ureq_serve("/", s_home, GET);
*/

char *s_home() {
    /* That's the most basic example. In this case, client will receive
     * text/html response containing only "home" string. */
    return "home";
}

char *s_home_plain(HttpRequest *r) {
    /* This works like s_home except string is sent as text/plain.
     * Please note, that this function takes HttpRequest as an argument. */
    r->response.mime = "text/plain";
    return "home";
}

char *s_header(HttpRequest *r) {
    /* This will redirect client to /test */
    r->response.code = 302;
    r->response.header = "Location: /test";
    return "";
}

char *s_param(HttpRequest *r) {
    /* This one shows how to handle GET parameters.
     * Please note, that ureq_get_param_value uses
     * common buffer for all operations, so store
     * copy data from it before calling it again */
    char *arg;

    strcpy(r->buffer, "data: ");
    arg = ureq_get_param_value(r, "data");
    strcat(r->buffer, arg);

    strcat(r->buffer, "<br>data2: ");
    arg = ureq_get_param_value(r, "data2");
    strcat(r->buffer, arg);

    return r->buffer;
}

char *s_all() {
    /* No mater which method client used, if he requested this one,
     * he'll always get it */
    return "All requests welcomed";
}

char *s_post(HttpRequest *r) {
    printf("PARAMS: %s\n", r->params);
    printf("BODY: %s\n", r->body);

    /*
    char *arg = ureq_get_param_value(r->params, "test2");

    if ( strcmp( arg, "2" ) == 0 ) {
        printf("POST OK!\n");
    }

    //free(arg);
    */

    return "OK";
}

char *s_buf() {
    return "test";
}

char *s_404() {
    return "Custom 404 page!";
}

/* --------------------------------^ PAGES ^-------------------------------- */

int main() {

    /*
    Before doing anything, set some urls to be dispatched when connection
    will come. First, set url, then corresponding function, then method
    that will be connected to that url.
    */
    ureq_serve("/", s_home, UREQ_GET);
    ureq_serve("/header", s_header, UREQ_GET);
    ureq_serve("/param", s_param, UREQ_GET);
    ureq_serve("/all", s_all, UREQ_ALL);
    ureq_serve("/post", s_post, UREQ_POST);
    ureq_serve("/buffer", s_buf, UREQ_GET);

    //ureq_serve("404", s_404, ALL);

    char request[] = "GET / HTTP/1.1\r\n"
                     "Host: 127.0.0.1:80\r\n";
    
    HttpRequest r = ureq_init(request);

    while(ureq_run(&r)) printf("%s\n", r.response.data);

    ureq_close(&r);
    ureq_finish();

    return 0;
}
