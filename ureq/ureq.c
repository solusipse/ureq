#include "ureq.h"

void ureq_get_header(char *h, char *r) {
    char *p = NULL;
    strncpy(h, r, strlen(r));
    char *b = strtok_r(h, "\n", &p);
    strncpy(h, b, strlen(b));
}

int ureq_parse_header(char *r, struct HttpRequest *req) {

    char *header = malloc( strlen(r) + 1 );
    header[0] = '\0';
    char *b = NULL;

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

    req->data = malloc( strlen(r) + 1 );
    req->data[0] = '\0';
    strncat(req->data, r, strlen(r));

    req->params = NULL;
    free(header);

    return 0;
}

void ureq_serve(char *url, char *(func)(char *), char *method ) {
    struct Page page;
    page.url = url;
    page.func = func;
    page.method = method;

    pages = (struct Page *) realloc(pages, ++pageCount * sizeof(struct Page) );
    pages[pageCount-1] = page;
}

void ureq_send(char *r) {
    printf("%s\n", r);
}

char *ureq_run( struct HttpRequest *req ) {
    
    int i;
    for (i = 0; i < pageCount; i++) {
        char *plain_url = malloc( strlen(req->url) + 1 );

        ureq_remove_parameters(plain_url, req->url);

        if ( strcmp(plain_url, pages[i].url) != 0 ) {
            free(plain_url);
            continue;
        }

        free(plain_url);

        // If request type is ALL, corresponding function is always called
        // no matter which method client has used.
        if ( strcmp(ALL, pages[i].method) != 0 )
            if ( strcmp(req->type, pages[i].method) != 0 ) {
                continue;
            }

        char *html = NULL;

        char *par = malloc( strlen(req->url) );
        ureq_get_parameters( par, req->url );

        if ( strcmp (POST, req->type ) == 0 ) {
            if (par != NULL && strcmp(par, req->url) != 0) {
                char *b = malloc(strlen(par) + strlen(req->data) + 2);
                // last character is newline which we want to remove
                strncat(b, req->data, strlen(req->data) - 1);
                strncat(b, "&", 1);
                strncat(b, par, strlen(par));
                html = pages[i].func( b );
                free(b);
            } else {
                html = pages[i].func( req->data );
            }
        } else {
            char *b = malloc(strlen(par) + strlen(req->data) + 2);
            strncat(b, req->data, strlen(req->data));
            strncat(b, "\n", 1);
            strncat(b, par, strlen(par));
            html = pages[i].func( b );
            free(b);
        }

        free(par);

        char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";

        // malloc -> calloc
        char *buf = malloc(strlen(header) + strlen(html) + 2);
        buf[0] = '\0';
        
        strcat(buf, header);
        strcat(buf, html);

        return buf;

    }
    
    return "404";
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

void ureq_get_parameters(char *b, char *u) {
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
    free(req->data);
}

void ureq_finish() {
    free(pages);
}