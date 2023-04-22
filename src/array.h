#ifndef _CKISS_ARRAY_H
#define _CKISS_ARRAY_H

#include <stdbool.h>

/* Array convenience functions. Arrays are lists of strings with a terminating
 * NULL. */

typedef char ** array_t;

/* Walk an array to find its length, excluding the terminating NULL record. */
size_t arr_len(array_t arr);

/* Appends to *arr (allocs if NULL). If you don't know the length of arr but it
 * has a terminating NULL, supply an n < 0. If s should be strdup'd then set dup
 * to true. */
array_t arr_append(array_t *arr, char *s, int n, bool dup);

/* Make a deep copy of an array. */
array_t arr_copy(array_t arr);

/* Free each element of arr and arr. */
void arr_free(array_t arr);

/* splits s by any delimiters in sep into an array of strings. Each string and
 * the array must be freed by the caller. */
array_t split(char *s, char *sep);

#endif
