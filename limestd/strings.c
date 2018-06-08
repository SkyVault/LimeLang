#include <stdlib.h>
#include <string.h>

typedef struct string {
    char* data;
    int len;
    long hash;
} string;

string make_empty_string() {
    string str = (string) {
        .data = malloc(1),
        .len = 1,
        .hash = 0
    };
    str.data[0] = '\0';
    return str;
}

string make_string(const char* cstr) {
    size_t len = strlen(cstr);
    string str = (string) {
        .data = malloc(len),
        .len = len,
        .hash = 0
    };
    for (int i = 0; i < len; i++)
        str.data[i] = cstr[i];
    str.data[len] = '\0';
    return str;
}
