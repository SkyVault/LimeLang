#include <stdlib.h>
#include <string.h>

typedef struct string {
    char* data;
    int len;
    long hash;
} string;

long str_hash(char* key) {
    long hash_val = 0;
    while(*key != '\0') {
        hash_val = (hash_val << 4) + *(key++);
        long g = hash_val & 0xF0000000L;
        if (g != 0) hash_val ^= g >> 24;
        hash_val &= ~g;
    }
    return hash_val;
}

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
