#include "ureq/ureq.c"

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

char *s_home(char *request) {
    printf("%s\n", request);
    return "home";
}

char *s_on() {
    return "on";
}

char *s_off() {
    return "off";
}

char *s_gettest() {
    return "off";
}

char *s_all() {
    return "all";
}

char *s_post(char *r) {

    char *data = ureq_get_params(r);
    char *arg = ureq_get_param_value(data, "test2");

    if ( strcmp( arg, "2" ) == 0 ) {
        printf("POST OK!\n");
    }
    
    free(data);
    free(arg);

    return "OK";
}

char *s_getpar(char *r) {

    char *data = ureq_get_params(r);
    char *arg = ureq_get_param_value(data, "test2");

    if ( strcmp( arg, "2ok" ) == 0 ) {
        printf("OK!\n");
    }
    
    free(data);
    free(arg);

    return "GETpar";
}

/* --------------------------------^ PAGES ^-------------------------------- */

int main() {

    // TODO: minimalize number of functions needed to use library

    /* 
    void ureq_serve(char *url, char *(func)(), int method );
    Bind functions to urls. Pass url, function name and method.
    */
    ureq_serve("/", s_home, GET);
    ureq_serve("/on", s_on, GET);
    ureq_serve("/off", s_off, POST);
    ureq_serve("/?test=ok", s_gettest, GET);
    ureq_serve("/all", s_all, ALL);
    ureq_serve("/post", s_post, POST);
    ureq_serve("/param", s_getpar, GET);

    /*
    That's an example request
    */
    
    /*
    char request[] = "POST /post?thisis=test HTTP/1.1\n"
                     "Host: 127.0.0.1:80\n\n"
                     "test=1&test2=2&test3=3\n";
    */

    
    char request[] = "GET /param?test=ok&test2=2ok HTTP/1.1\n"
                     "Host: 127.0.0.1:80\n";
    
    struct HttpRequest req;
    ureq_run(&req, request);
    printf("%s\n", req.response);
    ureq_close(&req);

    // ========================================================================
    // Second request

    /*

    char request2[] = "POST /post?thisis=test HTTP/1.1\n"
                     "Host: 127.0.0.1:80\n\n"
                     "test=1&test2=2&test3=3\n";
    
    // TODO: move ureq_parse_header inside ureq_run, save response to request struct
    struct HttpRequest req2;
    if ( ureq_parse_header(request2, &req2) != 0 )
        return 1;

    char *r2 = ureq_run(&req2);

    printf("%s\n", r2);
    free(r2);

    ureq_close(&req2);
    */

    ureq_finish();

    return 0;
}