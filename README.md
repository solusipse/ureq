# ureq
Micro C library for handling HTTP requests on low resource systems.

-------------------------------------------------------------------------------

### How does it work?

ureq is a middleware that sits between any kind of tcp server and the core of your application. Use it to **dispatch urls to functions**, like this: `ureq_serve("/", my_function, GET);`. It supports **basic http functionality** and can be used anywhere **system resources are low** (it was built for embedded systems, e.g. ESP8266) or where sophisticated http features aren't needed. And yes, **it understands RESTful**.

### Basic usage

As said before, it is used for dispatching. **Let's add a couple of urls then**:
```
ureq_serve("/", s_home, GET);
ureq_serve("/all", s_all, ALL);
ureq_serve("/post", s_post, POST);
```
How does it work? **When there's a request for an url** (`/`, `/all`, `/post`), **with corresponding method** (`GET`, `ALL`, `POST`), **ureq calls the corresponding function** (`s_home()`, `s_all()`, `s_post()`). What's `ALL` method? It calls a function connected to the url, no matter which type of method was used.

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

### License
See `LICENSE`.
