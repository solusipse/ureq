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

    return 0;
}

void ureq_serve(char *url, char *(func)(), int method ) {
    struct Page page;
    page.url = url;
    page.func = func;

    pages = (struct Page *) realloc(pages, pageCount++ * sizeof(struct Page) + 1);
    pages[pageCount-1] = page;
}

void ureq_send(char *r) {
    printf("%s\n", r);
}

void ureq_run( struct HttpRequest *req ) {
    int i;
    for (i = 0; i < pageCount; i++) {
        if ( strcmp(req->url, pages[i].url) == 0 ) {

            char *html = pages[i].func();
            char *header = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";

            char *buf = malloc(strlen(header) + strlen(html) + 2);
            buf[0] = '\0';
            
            strcat(buf, header);
            strcat(buf, html);

            ureq_send(buf);
            free(buf);
        }
    }
    // else: 404
}

void ureq_close() {
    free(pages);
}