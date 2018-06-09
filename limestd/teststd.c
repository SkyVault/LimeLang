#include "strings.c"
#include "arrays.c"

#include <stdio.h>

int main() {
    ARRAY_INIT(myArr);
    ARRAY_ADD(myArr, "Hello");
    ARRAY_ADD(myArr, ",");
    ARRAY_ADD(myArr, " ");
    ARRAY_ADD(myArr, "World");
    ARRAY_ADD(myArr, "!\n");

    for (int i = 0; i < ARRAY_TOTAL(myArr); i++){
        printf("%s", ARRAY_GET(myArr, char*, i));
    }

    ARRAY_FREE(myArr);
}
