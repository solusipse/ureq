# ureq
Micro C library for handling HTTP requests on low resource systems.

-------------------------------------------------------------------------------

## How does it work?

ureq is a middleware that sits between any kind of tcp server and the core of your application. Use it to **dispatch urls to functions**, like this: `ureq_serve("/", my_function, GET);`. It supports **basic http functionality** and can be used anywhere **system resources are low** (it was built for embedded systems, e.g. ESP8266) or where sophisticated http features aren't needed. And yes, **it understands RESTful**.

## Basic usage

As said before, it is used for dispatching. **Let's add a couple of urls then**:
```
ureq_serve("/", s_home, GET);
ureq_serve("/all", s_all, ALL);
ureq_serve("/post", s_post, POST);
```
How does it work? **When there's a request for an url** (`/`, `/all`, `/post`), **with corresponding method** (`GET`, `ALL`, `POST`), **ureq calls the corresponding function** (`s_home()`, `s_all()`, `s_post()`). What's `ALL` method? It calls a function connected to the url, no matter which type of method was used.

But wait, how should such function look like? The only requirement is that **it has to return a string (e.g. html code)**. For example:

```
char *some_function() {
    return "<h1>Some text!</h1>";
}
```

You want a **custom response** or **deal with request on your own**? No problem:

```
char *the_other_function(struct HttpRequest *r) {
    r->response = "HTTP/1.1 302 Found\nLocation: /\n";
    return "<h1>Some text!</h1>";
}
```

Keep in mind that adding argument to function definition is **not obligatory**.

**We connected some urls to functions**, what's next?

We have to **create a structure** that holds all the data. Simply put such line in your program:
```
struct HttpRequest req;
```
Then **run ureq's dispatching mechanism**:
```
ureq_run(&req, request);
```
**And that's all**, if request was correct, **there's a ready response in** `req.response` now. Want to check if it was? **Check what** `ureq_run` **has returned**. Was it `200`, `404`, `400`?

Perform a **cleanup after you're done** with that request:
```
ureq_close(&req);
```
Are you done with a server? Call this:
```
ureq_finish();
```

-------------------------------------------------------------------------------

And **that's** actually **all**, let's summarize:
```
struct HttpRequest req;
if (ureq_run(&req, buffer) == -1)
    return;

printf("%s\n", req.response);
write(socket, req.response, strlen(req.response));
ureq_close(&req);
```

-------------------------------------------------------------------------------

Remember to check out our **examples**!

## Detailed usage
To take **precise control of server's response**, modify **HttpRequest** struct's fields inside page connected to framwork via `ureq_serve`.

```
typedef struct HttpRequest {
    char *type;
    char *url;
    char *version;
    char *message;
    char *params;
    char *body;

    char *response;

    int  responseCode;
    char *responseDescription;
    char *responseHeaders;
    char *mime;
} HttpRequest;
```

Some of these fields are recieved from client, some are sended to client when processing's finished. There are some examples:

#### Set response code
```
r->responseCode = 302;
```

#### Set custom response header
```
r->responseHeaders = "Location: /";
```

#### Set response mime-type:
```
r->mime = "text/plain";
```

#### Get url parameters
```
r->params
```

## TODO
- create error handling system for dynamic allocated objects (server should ignore requests when memory is short without crashing, return 503 when possible)
- server should return 400 Bad Request instead of leaving this for user
- detailed documentation
- add macro for disabling dynamic memory allocation
- make ureq compatibile with esp8266

## License
See `LICENSE`.
