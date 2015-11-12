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
#include "internal/ureq_http_types.h"

/**
@brief: Register a function into the system that maps to a url with
        the corresponding http method

@url:    URL to associate with the specified function
@func:   Function to register into the system that maps to the url
@method: HTTP method
*/
void ureq_serve(char *url, char *(*func)(HttpRequest*), char *method);

/**
@brief: Retrieve an object from the system containing information
        about the request

@r: Character input stream containing the request
*/
HttpRequest ureq_init(const char *r);

/**
@brief: Process the request

@req: Object containing information about the request

@see: ureq_init
*/
int ureq_run(HttpRequest *req);

/**
@brief: Clean up system resources associated with the object

@req: Processed http request object

@see: ureq_run
*/
void ureq_close(HttpRequest *req);

/**
@brief: Shutdown the system to allow cleanup
*/
void ureq_finish();

/**
@brief:

@req:

@dst:

@cnt:
*/
void ureq_template(HttpRequest *req, char *dst, char *cnt);

#include "internal/ureq_impl.h"

#endif
