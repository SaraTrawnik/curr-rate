#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s);
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s);
