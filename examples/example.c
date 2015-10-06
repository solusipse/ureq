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

#include "../ureq.c"

/* --------------------------------v PAGES v-------------------------------- */

/*
These functions are called when corresponding request is issued by a client.

Bind these functions with ureq_serve function:
void ureq_serve(char *url, char *(func)(char *), char *method );
e.g.: ureq_serve("/", s_home, GET);

These function have to return strings which are basically html code added
to a header when ureq_run function is called, e.g.:

char *s_on() {
    return "on";
}

If you want to get some data from the request, add a char* argument to
function's definition, e.g.:

char *s_home(char *request) {
    printf("%s\n", request);
    return "home";
}

*/

/* --------------------------------v PAGES v-------------------------------- */

// TODO: add a functionality of returning custom headers

char *s_home() {
    return "home";
}

char *s_param(HttpRequest *r) {
    //r->responseCode = 302;
    //r->responseHeaders = "Location: /test";
    //r->mime = "text/plain";
    return "off";
}

char *s_all() {
    return "all";
}

char *s_post(HttpRequest *r) {
    printf("%s\n", r->params);
    printf("%s\n", r->body);

    /*
    char *arg = ureq_get_param_value(r->params, "test2");

    if ( strcmp( arg, "2" ) == 0 ) {
        printf("POST OK!\n");
    }

    //free(arg);
    */

    return "OK";
}

char *s_getpar(HttpRequest *r) {
    /*
    char *data = ureq_get_params(r);
    char *arg = ureq_get_param_value(data, "test2");

    if ( strcmp( arg, "2ok" ) == 0 ) {
        printf("OK!\n");
    }
    
    free(data);
    free(arg);
    */
    return "GETpar";
}

char *s_buf() {
    return "test";
}

/* --------------------------------^ PAGES ^-------------------------------- */

int main() {

    /*
    Before doing anything, set some urls to be dispatched when connection
    will come. First, set url, then corresponding function, then method
    that will be connected to that url.
    */

    ureq_serve("/", s_home, GET);
    ureq_serve("/param", s_param, GET);
    ureq_serve("/all", s_all, ALL);
    ureq_serve("/post", s_post, POST);
    ureq_serve("/buffer", s_buf, GET);

    /*
    That's just an example request.
    */
    char request[] = "GET /buffer HTTP/1.1\n"
                     "Host: 127.0.0.1:80\n";
    
    /*
    Before doing anything, initialize HttpRequest struct. Then call ureq_run
    with it as the first argument. Pass there also an incoming request.
    */
    HttpRequest r = ureq_init(request);
    /*
    ureq_run returns some codes, for example 200 or 404. If it returns -1,
    it means that the request was incorrect.
    */
    while(ureq_run(&r))
        printf("%s\n", r.response);
    /*
    Do something with generated response.
    */
    //printf("%s\n", r.response);
    /*
    When you're done with this particular request, remember to call ureq_close.
    */
    ureq_close(&r);
    /*
    When you're done with everything, call ureq_finish.
    */
    ureq_finish();

    return 0;
}
