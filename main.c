#include <json.h>
#include <curl/curl.h>
#include "str.h"

char *help = "curr-rate BASE [ CONVERT-TO ... ]\n\nmandatory args:\n    BASE        base to convert from\n    CONVERT-TO  converts BASE to following currencies\n";

#define BASE "base="
#define SYMBOLS "&symbols="
#define COMMA ","

void generate_url(struct string *s, int argcount, char **fullarg);
int get_json(struct string *a);
int read_json(struct string *s);

int main(int argc, char **argv) {
    if (argc < 3) { // needs at least two inputs
        fprintf(stderr, "%s\n", help);
        exit(EXIT_FAILURE);
    }

    struct string url;
    init_string(&url);
    generate_url(&url, argc, argv);
    get_json(&url);

    free(url.ptr);

    return 0;
}


void generate_url(struct string *query, int argcount, char **fullarg) {
    writefunc("https://api.exchangeratesapi.io/latest?", 1, strlen("https://api.exchangeratesapi.io/latest?"), query);
    writefunc(BASE, 1, strlen(BASE), query);
    writefunc(fullarg[1], 1, strlen(fullarg[1]), query);
    writefunc(SYMBOLS, 1, strlen(SYMBOLS), query);
    for (int i = 2; i < argcount; i++) {
        writefunc(fullarg[i], 1, strlen(fullarg[i]), query);
        writefunc(COMMA, 1, strlen(COMMA), query);
    }
    query->len -= 1;
    query->ptr[query->len] = '\0';
}

int get_json(struct string *url) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        struct string response;
        init_string(&response);

        curl_easy_setopt(curl, CURLOPT_URL, url->ptr); // set rates for now
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed\n");
        curl_easy_strerror(res);

        read_json(&response); 

        // string cleanup
        free(response.ptr);

        // curl cleanup
        curl_easy_cleanup(curl);
    }
    
    curl_global_cleanup();
}

int read_json(struct string *resp) {
    struct json_object *jobj; // entire json
    struct json_object *base; // base currency
    struct json_object *rates; // currencies base is oncerted to

    jobj = json_tokener_parse(resp->ptr);

    json_object_object_get_ex(jobj, "base", &base);
    printf("%s 1.00\n", json_object_get_string(base));

    json_object_object_get_ex(jobj, "rates", &rates);
    json_object_object_foreach(rates, key, val) {
        printf("%s %s\n", key, json_object_get_string(val));
    }

    // json cleanup
    json_object_put(jobj);
    json_object_put(base);
    json_object_put(rates);

    return 0;
}
