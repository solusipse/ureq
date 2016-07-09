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

#ifndef UREQ_PAGES_H
#define UREQ_PAGES_H

const char* UREQ_HTML_HEADER   = "<html><head><title>";

const char* UREQ_HTML_BODY     = "</title></head><body><center><h1>";

const char* UREQ_HTML_FOOTER   = "</h1><hr>ureq</center></body></html>";

const char* UREQ_HTML_PAGE_404 = "<html>"
                                 "<head><title>404 Not Found</title></head>"
                                 "<body><center><h1>404 Not Found</h1><hr>ureq</center></body>"
                                 "</html>";

#endif /* UREQ_PAGES_H */
