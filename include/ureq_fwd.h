/*
https://github.com/solusipse/ureq

The MIT License (MIT)

Copyright (c) 2015-2016 solusipse

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

#ifndef UREQ_FWD_DECLS_H
#define UREQ_FWD_DECLS_H

void               ureq_serve                    (char*, char *(*)(HttpRequest*), char*);
HttpRequest        ureq_init                     (const char*);
void               ureq_close                    (HttpRequest*);
void               ureq_finish                   ();
void               ureq_template                 (HttpRequest*, char*, char*);
int                ureq_run                      (HttpRequest*);

static int         ureq_get_header               (char*, const char*);
static int         ureq_parse_header             (HttpRequest*, const char*);
static void        ureq_remove_parameters        (char*, const char*);
static void        ureq_get_query                (HttpRequest*);
static void        ureq_generate_response        (HttpRequest*, char*);
static void        ureq_set_post_data            (HttpRequest*);
static char       *ureq_set_mimetype             (const char*);
static char       *ureq_generate_response_header (HttpRequest*);
static const char *ureq_get_code_description     (const int);
static int         ureq_set_error_response       (HttpRequest*);
static int         ureq_set_404_response         (HttpRequest*);

#endif /* UREQ_FWD_DECLS_H */
