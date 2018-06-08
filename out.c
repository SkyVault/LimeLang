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

int factorial(int n);
int factorial(int n){
    if ((n == 0)) {
        return (1);
    }
    else {
        return (n * factorial((n - 1)));
    }

}
int count;
int main(int num_arg_pkDHTx, char** args_pkDHTx) {
    count = (10);
    while ((count >= 1)) {
        printf(("!%d = %d\n"), (count), (factorial((count))));
    count = (count - 1);

    }
    return 0;
}

