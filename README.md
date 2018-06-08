![](logo.png)

## The refreshing programming language

This programming language is under heavy construction, but heres a snippet of whats possible...

```nim
Fib proc(n int) int {
    if n == 0 {
        ret n
    }

    if n == 1 {
        ret n
    }
    
    ret Fib(n - 2) + Fib(n - 1)
}

count int = 0
while count <= 42 {

    printf("count: %d = %d\n", count, Fib(count))

    count = count + 1
}
```
