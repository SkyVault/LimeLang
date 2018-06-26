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
i64 Add(i64 a, i64 b);
i64 Fact(i64 n){
    if ((n == 0)) {
        return (1);
    }
    else {
        return (n * Fact((n - 1)));
    }

}
i64 Add(i64 a, i64 b){
    return (a + b);
}
i32 myVar;
i16 test;
int main(int num_arg_p1KaLm, char** args_p1KaLm) {
    myVar = (Add((10), (20)));
    test = (Fact((5)));
    Fact((4), (5));
    return 0;
}

