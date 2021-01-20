#include <stdlib.h>
#include <pthread.h>

/*
 * THREAD SAFE VECTOR IMPLEMENTATION
 * 
 * vector implementation in C that is built to work in multi-threaded
 * environments where many threads are accessing the vector simultaneously. 
 * Takes pointers to desired data types and makes shallow copies of all 
 * input. All comparisons done by address not value.
 * Implementation by Charlie Rehder (Github: charlierehder)
 */ 

#define INITIAL_CAPACITY 8
#define GROWTH_FACTOR 2

/*
 * Vector struct that holds the size of the vector, the potential size (capcity)
 * of the vector without resizing, the array of data that vector holds and the 
 * mutex that protects the critical sections of the vector during multithreaded 
 * use. 
 */
typedef struct {

    size_t size;
    size_t capacity;
    void **data; 
    pthread_mutex_t m;

} vector;

/*
 * Initializes vector struct and returns pointer to newly allocated vector.
 * Size is initially zero and vector has no contents.
 * 
 * @return: new, empty vector
 */
vector *vector_create();

/*
 * Destroys data associated with vector. Every element pushed into vector
 * by the user will not be freed. It is the responsibility of the user to 
 * free all data inside the vector. Ideally this will be called after
 * freeing every element in the vector.
 * 
 * @param: vector to free
 */
void vector_destroy(vector *vec);

/*
 * Adds desired element to the vector at the desired index. This moves each
 * element with an index larger than the provided index up by one. Only
 * indices with value less than the size of the vector will be accepted.
 * 
 * @param: index at which the given element will be placed
 * @param: element that will be placed at the given index
 */ 
void vector_add(vector *vec, size_t index, void *elem);

/*
 * Remove element of vector at that index. Keep in mind the vector is made
 * of shallow copies of the data given so you'll still need to free that 
 * memory either before or after removing it from the vector, that is if 
 * you want to get rid of the data completely.
 * 
 * @param: index of element to be removed
 */
void vector_remove(vector *vec, size_t index);

/*
 * Replaces the element at the desired index with the element that it is
 * given. This does not free any data so it is recommended to free the prior
 * data at that index before setting it to another value. Only indices that
 * are less than the overall size of the vector will be accepted.
 * 
 * @param: index in the vector that will hold the given element
 * @param: elem to place at the given index
 */
void vector_set(vector *vec, size_t index, void *elem);

/*
 * Inserts element at the end of given vector. 
 * 
 * @param: element to store in vector
 */
void vector_push_back(vector *vec, void *elem);

/*
 * Returns pointer to element at of given vector at specified index. Same pointer 
 * that was used to define element.
 * 
 * @param: index of desired element (0-indexed)
 * @return: pointer to element at index. 
 */
void *vector_get(vector *vec, size_t index);

/*
 * Returns the size of the given vector (max index + 1)
 * 
 * @return: size of vector as size_t
 */
size_t vector_size(vector *vec);

/*
 * Returns whether the vector is empty or not.
 * 
 * @return: 1 if empty, 0 if not empty
 */
int vector_empty(vector *vec);