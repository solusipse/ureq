#include "ureq.h"

void ureq_get_header(char *h, char *r) {
    strncat(h, r, strlen(r));
    h = strtok(h, "\n");
}

int ureq_parse_header(char *r, struct HttpRequest *req) {

    char *header = malloc( strlen(r) + 1 );
    char *b = malloc( strlen(r) + 1);

    ureq_get_header(header, r);
    b = strtok(header, " ");
    free(header);

    if ( strncmp(b, "GET", strlen(b) + 8) && strncmp(b, "POST", strlen(b) + 8) )
        return 1;
    req->type = malloc( strlen(b) + 1 );
    strncat(req->type, b, strlen(b));

    b = strtok(NULL, " ");
    req->url = malloc( strlen(b) + 1 );
    strncat(req->url, b, strlen(b));

    b = strtok(NULL, " ");
    if ( strncmp(b, "HTTP/1.1", strlen(b) + 8) && strncmp(b, "HTTP/1.0", strlen(b) + 8) )
        return 1;
    req->version = malloc( strlen(b) + 1 );
    strncat(req->version, b, strlen(b));

    b = strtok(NULL, " ");
    if (b != NULL)
        return 1;

    req->data = malloc( strlen(r) + 1 );
    strncat(req->data, r, strlen(r));

    req->params = NULL;
    free(b);

    return 0;
}

void ureq_serve(char *url, char *(func)(char *), char *method ) {
    struct Page page;
    page.url = url;
    page.func = func;
    page.method = method;

    // TODO: fix memory problems below (minimalize memory footprint)
    //       (malloc every element separately)
    pages = (struct Page *) realloc(pages, ++pageCount * sizeof(struct Page) );
    pages[pageCount-1] = page;
}

void ureq_send(char *r) {
    printf("%s\n", r);
}

void ureq_run( struct HttpRequest *req ) {
    
    int i;
    for (i = 0; i < pageCount; i++) {
        char *plain_url = ureq_remove_parameters(req->url);

        if ( strcmp(plain_url, pages[i].url) != 0 )
            continue;

        // If request type is ALL, corresponding function is always called
        // no matter which method client has used.
        if ( strcmp(ALL, pages[i].method) != 0 )
            if ( strcmp(req->type, pages[i].method) != 0 )
                continue;

        char *html = NULL;

        char *par = malloc( strlen(req->url) );
        ureq_get_parameters( par, req->url );

        if ( strcmp (POST, req->type ) == 0 ) {
            if (par != NULL) {
                char *b = malloc(strlen(par) + strlen(req->data) + 2);
                strcat(b, par);
                strcat(b, req->data);
                html = pages[i].func( par );
            } else {
                html = pages[i].func( req->data );
            }
        } else {
            html = pages[i].func( par );
        }


            
        char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";

        // malloc -> calloc
        char *buf = malloc(strlen(header) + strlen(html) + 2);
        buf[0] = '\0';
        
        strcat(buf, header);
        strcat(buf, html);

        ureq_send(buf);
        free(buf);
        free(par);

    }
    
    // else: 404
}

char *ureq_get_post_arguments(char *r) {
    char *data = malloc(strlen(r) + 1);
    char *out = malloc(strlen(r) + 1);
    strcpy(data, r);

    for (char *buf = strtok(data,"\n"); buf != NULL; buf = strtok(NULL, "\n")) {
        strcpy(out, buf);
    }
    free(data);

    return out;
}

char *ureq_get_argument_value(char *r, char *arg) {
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

char *ureq_remove_parameters(char *u) {
    char buf[128];
    strcpy(buf, u);
    return strtok(buf, "?");
}

void ureq_get_parameters(char *b, char *u) {
    strcpy(b, u);
    b = strtok(b, "?");
    b = strtok(NULL, "\n");
}

void ureq_close( struct HttpRequest *req ) {
    free(req->type);
    free(req->url);
    free(req->version);
    free(req->data);

    if (req->params != NULL)
        free(req->params);

    free(pages);
}