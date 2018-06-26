// Lime Version: 0.0.0
#include <stdio.h>
#include "arrays.c"
#define none NULL
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long   u64;

typedef int             i8;
typedef int             i16;
typedef int             i32;
typedef int             i64;

typedef float           f32;
typedef double          f64;
//typedef long            int;
typedef unsigned int    uint;
typedef float           real;

i64 Fact(i64 n);
i64 Fact(i64 n){
    if ((n == 0)) {
        return (1);
    }
    else {
        return (n * Fact((n - 1)));
    }

}
i64 variable;
i64 mutableVar;
i64 expression;
i64 myArray;
i64 count;
int main(int num_arg_p1KaLm, char** args_p1KaLm) {
    variable = (123);
    mutableVar = (32);
    expression = (123 + (32 / 2.0) * 2);
    ARRAY_INIT(myArray);
    ARRAY_ADD(myArray, (100));
    ARRAY_ADD(myArray, (200));
    ARRAY_ADD(myArray, (300));
    count = (0);
    while ((count <= 42)) {
        count += (1);

    }
    return 0;
}

