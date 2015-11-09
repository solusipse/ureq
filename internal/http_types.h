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

#ifndef UREQ_HTTP_TYPES_H
#define UREQ_HTTP_TYPES_H

/*
*/
typedef struct ureq_mime_t {
    const char *ext;
    const char *mime;
} UreqMime;

/*
*/
typedef struct ureq_file_t {
    int size;
    int address;
} UreqFile;

/*
*/
typedef struct ureq_response_t {
    int  code;
    char *mime;
    char *header;
    char *data;
    char *file;
} UreqResponse;

/*
*/
typedef struct ureq_template_t {
    char *destination;
    char *value;
} UreqTemplate;

/*
*/
typedef struct ureq_http_request_t {
    char *type;
    char *url;
    char *version;
    char *message;
    char *params;
    char *body;

    UreqResponse response;

    UreqTemplate templates[16];
    int tmp_len;

    int complete;
    int big_file;
    int len;

    UreqFile file;
    // TODO: use another buffer for backend operations
    // leave this one for user
    char buffer[UREQ_BUFFER_SIZE];
    char _buffer[UREQ_BUFFER_SIZE];

    int valid;

    /* Type Methods */
    char *(*func)(struct ureq_http_request_t*);
    char *(*page404)(struct ureq_http_request_t*);
} HttpRequest;

/*
*/
typedef struct ureq_page_t {
    char *url;
    char *method;

    /* Type Methods */
    char *(*func)();
} UreqPage;

#endif /* UREQ_HTTP_TYPES_H */