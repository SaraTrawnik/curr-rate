#include <json.h>
#include <curl/curl.h>
#include "str.h"

char *help = "curr-rate BASE [ CONVERT-TO ... ]\n\nmandatory args:\n    BASE        base to convert from\n    CONVERT-TO  converts BASE to following currencies\n";

#define BASE "base="
#define SYMBOLS "&symbols="
#define COMMA ","

void generate_url(struct string *s, int argcount, char **fullarg);

int main(int argc, char **argv) {
    if (argc < 3) { // needs at least two inputs
        fprintf(stderr, "%s\n", help);
        exit(EXIT_FAILURE);
    }

    struct string url;
    generate_url(&url, argc, argv);

    struct json_object *jobj;
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        struct string s;
        init_string(&s);

        curl_easy_setopt(curl, CURLOPT_URL, url.ptr); // set rates for now
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed\n");
        curl_easy_strerror(res);

        jobj = json_tokener_parse(s.ptr);
        printf("%s\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
        
        free(url.ptr);
        free(s.ptr);
        curl_easy_cleanup(curl);
    }
    
    curl_global_cleanup();
    return 0;
}


void generate_url(struct string *s, int argcount, char **fullarg) {
    init_string(s);
    writefunc("https://api.exchangeratesapi.io/latest?", 1, strlen("https://api.exchangeratesapi.io/latest?"), s);

    writefunc(BASE, 1, strlen(BASE), s);
    writefunc(fullarg[1], 1, strlen(fullarg[1]), s);
    writefunc(SYMBOLS, 1, strlen(SYMBOLS), s);
    for (int i = 2; i < argcount; i++) {
        writefunc(fullarg[i], 1, strlen(fullarg[i]), s);
        writefunc(COMMA, 1, strlen(COMMA), s);
    }
    s->len -= 1;
    s->ptr[s->len] = '\0';
}
