#include <mem.h>
#include <osapi.h>

#define realloc ureq_realloc
#define malloc  ureq_malloc
#define free    ureq_free

#define printf(...) os_printf( __VA_ARGS__ )
#define sprintf(...) os_sprintf( __VA_ARGS__ )

char *ureq_malloc(size_t l) {
    return (char *) os_malloc(l);
}

void ureq_free(void *p) {
    if (p != NULL) {
        os_free(p);
        p = NULL;
    }
}
