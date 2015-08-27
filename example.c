#include "ureq.c"

/* --------------------------------v PAGES v-------------------------------- */

/* These functions (s_*) are used for page logic. Every such
   function that is passed to serve() has to return a
   string. This string will be sent to client as a html
   code.
*/

char *s_home() {
    // TODO: pass request
    return "home";
}

char *s_on() {
    printf("on\n");
    return "abc";
}

char *s_off() {
    printf("off\n");
    return "abc";
}

/* --------------------------------^ PAGES ^-------------------------------- */

int main() {

    /*
    That's an example request
    */

    char request[] = "GET / HTTP/1.1\n"
                     "Host: 127.0.0.1:80\n";

    struct HttpRequest req;
    if ( ureq_parse_header(request, &req) != 0 )
        return 1;

    /* 
    void ureq_serve(char *url, char *(func)(), int method );
    Bind functions to urls. Pass url, function name and method.
    */

    ureq_serve("/", s_home, GET);
    ureq_serve("/on", s_on, GET);
    ureq_serve("/off", s_off, POST);
    /*
    void ureq_run(struct HttpRequest *req);
    Use ureq_run everytime you get a request.
    */

    ureq_run(&req);

    /*
    When you're done, use ureq_close.
    */

    ureq_close();

    return 0;
}