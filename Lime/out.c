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

int Fact(int n);
int Add(int a, int b);
int Fact(int n){
    if ((n == 0)) {
        return (1);
    }
    else {
        return (n * Fact((n - 1)));
    }

}
int Add(int a, int b){
    return (a + b);
}
int main(int num_arg_p1KaLm, char** args_p1KaLm) {
    printf(("!4 = %d, !5 = %d, !4 + !5 = %d"), (Fact((4))), (Fact((5))), (Add((Fact((4))), (Fact((5))))));
    return 0;
}

