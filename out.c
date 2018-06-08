// Lime Version: 0.0.0
#include <stdio.h>
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

int Fib(int n);
int Fib(int n){
    if ((n == 0)) {
        return (n);
    }
    if ((n == 1)) {
        return (n);
    }
    return (Fib((n - 2)) + Fib((n - 1)));
}
int count;
int main(int num_arg_pkDHTx, char** args_pkDHTx) {
    count = (0);
    while ((count <= 42)) {
        printf(("count: %d = %d\n"), (count), (Fib((count))));
    count = (count + 1);

    }
    return 0;
}

