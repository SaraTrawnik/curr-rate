#include <json.h>
#include <curl/curl.h>
#include "str.h"

int main(void) {
    struct json_object *jobj;
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        struct string s;
        init_string(&s);

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.exchangeratesapi.io/latest?symbols=GBP,USD"); // set rates for now
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed\n");
        curl_easy_strerror(res);

        jobj = json_tokener_parse(s.ptr);
        printf("%s\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
        free(s.ptr);
        curl_easy_cleanup(curl);
    }
    
    curl_global_cleanup();
    return 0;
}
