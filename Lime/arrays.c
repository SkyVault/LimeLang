#include <stdio.h>
#include <stdlib.h>

#define array_INIT_CAPACITY 4

#define ARRAY_INIT(vec) array vec; array_init(&vec)
#define ARRAY_ADD(vec, item) array_add(&vec, (void *) item)
#define ARRAY_SET(vec, id, item) array_set(&vec, id, (void *) item)
#define ARRAY_GET(vec, type, id) (type) array_get(&vec, id)
#define ARRAY_DELETE(vec, id) array_delete(&vec, id)
#define ARRAY_TOTAL(vec) array_total(&vec)
#define ARRAY_FREE(vec) array_free(&vec)

typedef struct array {
    void **items;
    int capacity;
    int total;
} array;

void array_init(array *);
int array_total(array *);
static void array_resize(array *, int);
void array_add(array *, void *);
void array_set(array *, int, void *);
void *array_get(array *, int);
void array_delete(array *, int);
void array_free(array *);

void array_init(array *v)
{
    v->capacity = array_INIT_CAPACITY;
    v->total = 0;
    v->items = malloc(sizeof(void *) * v->capacity);
}

int array_total(array *v)
{
    return v->total;
}

static void array_resize(array *v, int capacity)
{
    #ifdef DEBUG_ON
    printf("array_resize: %d to %d\n", v->capacity, capacity);
    #endif

    void **items = realloc(v->items, sizeof(void *) * capacity);
    if (items) {
        v->items = items;
        v->capacity = capacity;
    }
}

void array_add(array *v, void *item)
{
    if (v->capacity == v->total)
        array_resize(v, v->capacity * 2);
    v->items[v->total++] = item;
}

void array_set(array *v, int index, void *item)
{
    if (index >= 0 && index < v->total)
        v->items[index] = item;
}

void *array_get(array *v, int index)
{
    if (index >= 0 && index < v->total)
        return v->items[index];
    return NULL;
}

void array_delete(array *v, int index)
{
    if (index < 0 || index >= v->total)
        return;

    v->items[index] = NULL;

    for (int i = index; i < v->total - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->total--;

    if (v->total > 0 && v->total == v->capacity / 4)
        array_resize(v, v->capacity / 2);
}

void array_free(array *v)
{
    free(v->items);
}
