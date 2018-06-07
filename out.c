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
void Test();
int Add(int a, int b){
    return (a + b);
}
void Test(){

}
int myVar;
int main(int num_arg_pkDHTx, char** args_pkDHTx) {
    myVar = (Add((10), (2)));
    if ((myVar == 12)) {
        printf("Yass\n");
    }
    return 0;
}

