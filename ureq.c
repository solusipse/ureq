#include "ureq.h"

char *ureq_get_header(char *r) {
    char *buf = malloc(strlen(r) + 1);
    strcpy(buf, r);
    free(buf);
    return strtok(buf, "\n");
}

int ureq_parse_header(char *r, struct HttpRequest *req) {

    char *header = ureq_get_header(r);

    if (header == NULL)
        return 1;

    header = strtok(header, " ");

    if ( strncmp(header, "GET", strlen(header) + 8) && strncmp(header, "POST", strlen(header) + 8) )
        return 1;
    req->type = header;

    header = strtok(NULL, " ");
    req->url = header;

    header = strtok(NULL, " ");
    if ( strncmp(header, "HTTP/1.1", strlen(header) + 8) && strncmp(header, "HTTP/1.0", strlen(header) + 8) )
        return 1;
    req->version = header;

    header = strtok(NULL, " ");
    
    if (header != NULL)
        return 1;

    req->data = r;

    return 0;
}

void ureq_serve(char *url, char *(func)(char *), char *method ) {
    // TODO: parse get parameters
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
        char *plain_url = ureq_remove_parameters( req->url );

        if ( strcmp(plain_url, pages[i].url) != 0 )
            continue;

        // If request type is ALL, corresponding function is always called
        // no matter which method client has used.
        if ( strcmp(ALL, pages[i].method) != 0 )
            if ( strcmp(req->type, pages[i].method) != 0 )
                continue;

        char *html = NULL;

        if ( strcmp (POST, req->type ) == 0 ) {
            char *par = ureq_get_parameters( req->url );
            if (par != NULL) {
                char *b = malloc(strlen(par) + strlen(req->data) + 2);
                strcat(b, par);
                strcat(b, req->data);
                html = pages[i].func( par );
                free(b);
            } else {
                html = pages[i].func( req->data );
            }
        } else {
            html = pages[i].func( ureq_get_parameters( req->url ) );
        }
            
        char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";

        // malloc -> calloc
        char *buf = malloc(strlen(header) + strlen(html) + 2);
        buf[0] = '\0';
        
        strcat(buf, header);
        strcat(buf, html);

        ureq_send(buf);
        free(buf);

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
    free(out);

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
    free(out);

    return out;
}

char *ureq_remove_parameters(char *u) {
    char buf[128];
    strcpy(buf, u);
    return strtok(buf, "?");
}

char *ureq_get_parameters(char *u) {
    char *buf = malloc(strlen(u) + 1);
    strcpy(buf, u);
    buf = strtok(buf, "?");
    buf = strtok(NULL, "\n");
    return buf;
}

void ureq_close() {
    free(pages);
}