#include <json.h>
#include <curl/curl.h>
#include "str.h"

char *help_str = "curr-rate [-hl] BASE [ CONVERT-TO ... ]\n\nmandatory args:\n    BASE        base to convert from\n    CONVERT-TO  converts BASE to following currencies\n\noptional args:\n    -l          list of available currencies\n    -h          show help\n";

#define BASE "base="
#define SYMBOLS "&symbols="
#define COMMA ","

#define SHOW_PRICE 1
#define NO_PRICE 0

void generate_url(struct string *s, int argcount, char **fullarg);
int get_json(char *url, int show_prices);
void read_json(char *resp, int show_prices);

int main(int argc, char **argv) {
    int list = 0;
    int help = 0;

    if (argc < 2) { // needs at least one input 
        printf("%s\n", help_str);
        exit(EXIT_FAILURE);
    }
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-l"))
            list = 1;
        if (!strcmp(argv[i], "-h"))
            help = 1;
    }

    struct string url;
    init_string(&url);

    if (help) {
        printf("%s\n", help_str);
        return 0;
    }
    if (list) {
        writefunc("https://api.exchangeratesapi.io/latest?base=USD", 1, strlen("https://api.exchangeratesapi.io/latest?base=USD"), &url);
        get_json(url.ptr, NO_PRICE);
        return 0;
    }

    generate_url(&url, argc, argv);
    if (get_json(url.ptr, SHOW_PRICE)) {
        fprintf(stderr, "get_json() failed\n");
        exit(EXIT_FAILURE);
    }

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
    query->len -= 1; //remove the last comma
    query->ptr[query->len] = '\0';
}

int get_json(char *url, int show_prices) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        struct string response;
        init_string(&response);

        curl_easy_setopt(curl, CURLOPT_URL, url); // set rates for now
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed\n");
        curl_easy_strerror(res);

        read_json(response.ptr, show_prices);
        
        free(response.ptr)
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "curl_easy_init() failed\n");
        return 1;
    }
    
    curl_global_cleanup();
    return 0;
}

void read_json(char *resp, int show_prices) {
    struct json_object *jobj; // entire json
    struct json_object *base; // base currency
    struct json_object *rates; // currencies base is oncerted to

    jobj = json_tokener_parse(resp);

    json_object_object_get_ex(jobj, "base", &base);
    printf("%s 1.00\n", json_object_get_string(base));

    json_object_object_get_ex(jobj, "rates", &rates);
    json_object_object_foreach(rates, key, val) {
        char *show = show_prices ? json_object_get_string(val) : "";
        printf("%s %s\n", key, show);
    }

    // json cleanup
    json_object_put(jobj);
    json_object_put(base);
    json_object_put(rates);
}
