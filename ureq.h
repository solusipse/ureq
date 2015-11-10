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

#ifndef UREQ_H
#define UREQ_H

#include "internal/ureq_defines.h"
#include "internal/http_types.h"

void ureq_serve(char *url, char *(*func)(HttpRequest *), char *method);

HttpRequest ureq_init(const char *r);

void ureq_close( struct HttpRequest *req );
void ureq_finish();

void ureq_template(HttpRequest *r, char *dst, char *cnt);

int ureq_run(struct HttpRequest *req);

static int  ureq_get_header(char *h, const char *r);
static int  ureq_parse_header(struct HttpRequest *req, const char *r);
static void ureq_remove_parameters(char *b, const char *u);
static void ureq_get_query(HttpRequest *r);
static void ureq_generate_response(HttpRequest *r, char *html);
static void ureq_set_post_data(HttpRequest *r);

static char *ureq_set_mimetype(const char *r);
static char *ureq_generate_response_header(HttpRequest *r);
static char *ureq_get_code_description(int c);

static int ureq_set_error_response(HttpRequest *r);
static int ureq_set_404_response(HttpRequest *r);

#include "internal/ureq.impl"

#endif
