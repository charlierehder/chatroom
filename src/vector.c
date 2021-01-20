#include "vector.h"
#include <string.h>

vector *vector_create() {

    vector *nvec = malloc(sizeof(vector));
    nvec->size = 0;
    nvec->capacity = INITIAL_CAPACITY;
    nvec->data = malloc(nvec->capacity * sizeof(void *));
    memset(nvec->data, 0, INITIAL_CAPACITY * sizeof(void *));
    pthread_mutex_init(&nvec->m, NULL);
    return nvec;

}

void vector_destroy(vector *vec) {

    if (!vec) { return; }
    free(vec->data);
    vec->data = NULL;
    free(vec);
    vec = NULL;

}

// helper function -- not defined in .h
// must be called when vector mutex is locked
void vector_resize(vector *vec, size_t target) {

    if (!vec) { return; }

    size_t ncapacity = target;
    void **ndata = malloc(ncapacity * sizeof(void *));
    memset(ndata, 0, ncapacity * sizeof(void *));
    if (vec->capacity < ncapacity) {
        memcpy(ndata, vec->data, vec->capacity * sizeof(void *));
    } else if (vec->capacity >= ncapacity) {
        memcpy(ndata, vec->data, ncapacity * sizeof(void *));
    }
    free(vec->data); 
    vec->data = ndata;
    vec->capacity = ncapacity;

}

void vector_add(vector *vec, size_t index, void *elem){
    
    pthread_mutex_lock(&vec->m);

    if (!vec) { return; }
    if (index >= vec->size) { return; }

    if (vec->size + 1 > vec->capacity) {
        vector_resize(vec, vec->capacity * GROWTH_FACTOR);
    }
    size_t cindex = vec->size - 1;
    void *next;
    while(cindex >= index) {
        vec->data[cindex + 1] = vec->data[cindex];
        cindex--;
    }
    vec->data[index] = elem;
    vec->size++;

    pthread_mutex_unlock(&vec->m);

}

void vector_remove(vector *vec, size_t index) {

    pthread_mutex_lock(&vec->m);

    size_t cindex = index;
    while (cindex < vec->size) {
        vec->data[cindex] = vec->data[cindex + 1];
        cindex++;
    }
    vec->data[vec->size - 1] = NULL;
    vec->size--;

    pthread_mutex_unlock(&vec->m);

}

void vector_set(vector *vec, size_t index, void *elem) {

    pthread_mutex_lock(&vec->m);

    if (!vec) { return; }
    if (index >= vec->size) { return; }

    vec->data[index] = elem;

    pthread_mutex_unlock(&vec->m);

}

void vector_push_back(vector *vec, void *elem) {

    pthread_mutex_lock(&vec->m);

    if (!vec) { return; }

    if (vec->size + 1 > vec->capacity) {
        vector_resize(vec, vec->capacity * GROWTH_FACTOR);
    }
    vec->data[vec->size] = elem;
    vec->size++;

    pthread_mutex_unlock(&vec->m);

}

void *vector_get(vector *vec, size_t index) {

    pthread_mutex_lock(&vec->m);

    if (!vec) { return NULL; }
    if (vec->size <= index) { return NULL; }

    void *retval = vec->data[index];

    pthread_mutex_unlock(&vec->m);

    return retval;
    
}

size_t vector_size(vector *vec) {

    pthread_mutex_lock(&vec->m);
    size_t retval = vec->size;
    pthread_mutex_unlock(&vec->m);

    return retval;
}

int vector_empty(vector *vec) {

    pthread_mutex_lock(&vec->m);
    int retval = (vec->size != 0);
    pthread_mutex_unlock(&vec->m);
    return retval;
}
