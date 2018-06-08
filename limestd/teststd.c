#include "strings.c"
#include <stdio.h>

int main() {
    string str = make_empty_string();
    string str_2 = make_string("Hello World");

    printf("-->%s<--\n", str.data);
    printf("-->%s<--\n", str_2.data);
}
