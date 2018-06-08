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

int Add(int a, int b);
int Add(int a, int b){
    return (a + b);
}
int myVariable;
int main(int num_arg_pkDHTx, char** args_pkDHTx) {
    myVariable = (Add((10), (20)));
    printf(("10 + 20 = %d\n"), (myVariable));
    return 0;
}

