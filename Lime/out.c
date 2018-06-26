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

int g;
int b;
int main(int num_arg_p1KaLm, char** args_p1KaLm) {
    ARRAY_INIT(g);
    b = (1);
    ARRAY_ADD(g, (b));
    b = (b + 1);
    ARRAY_ADD(g, (b));
    printf("%d\n", ARRAY_GET(g, int, 1));    return 0;
}

