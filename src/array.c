#include <stdlib.h>
#include <string.h>

#include "ckiss.h"
#include "array.h"
#include "utils.h"

size_t
arr_len(array_t arr) {
    size_t n = 0;
    if (arr) {
        while (*arr != NULL) {
            arr++;
            n++;
        }
    }
    return n;
}

// TODO create if arr == NULL
array_t
arr_append(array_t *arr, char *s, int n, bool dup) {
    if (n < 0) {
        n = arr_len(*arr);
    }
    array_t tmp = realloc(*arr, sizeof(char *) * (++n + 1));
    if (tmp == NULL) {
        free(*arr);
        die_perror("realloc");
    }
    *arr = tmp;
    (*arr)[n - 1] = dup ? strdup(s) : s;
    (*arr)[n] = NULL;
    return *arr;
}

array_t
arr_copy(array_t arr) {
    size_t n = arr_len(arr);
    array_t s = malloc(sizeof(char *) * (n + 1));
    if (s == NULL)
        die_perror("malloc");
    for (size_t i = 0; i < n; i++)
        s[i] = strdup(arr[i]);
    s[n] = NULL;
    return s;
}

void
arr_free(array_t arr) {
    array_t t = arr;
    if (t) {
        while (*t) {
            free(*t);
            t++;
        }
        free(arr);
    }
}

array_t
split(char *s, char *sep) {
    if (s == NULL)
        return NULL;

    /* avoid modifying original string */
    char *ss = strdup(s);

    array_t res = NULL;
    char *p = strtok(ss, sep);
    int n = 0;

    while (p) {
        arr_append(&res, p, n++, true);
        p = strtok(NULL, sep);
    }

    free(ss);

    return res;
}
