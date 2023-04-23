#include <stdio.h>
#include <stdlib.h>

#include "ckiss.h"
#include "array.h"
#include "utils.h"

int
search(int argc, char **argv, struct env *e) {
    /* make a copy of kiss_path and append sys_db to it */
    array_t p = arr_copy(e->kiss_path);
    arr_append(&p, e->sys_db, -1, true);

    for (int i = 1; i < argc; i++) {
        array_t s = find_in_path(argv[i], p, S_IFDIR, false, true);

        if (s == NULL)
            die2(argv[i], "not found");

        for (int j = 0; s[j] != NULL; j++) {
            printf("%s\n", s[j]);
            free(s[j]);
        }

        free(s);
    }
    arr_free(p);
    return 0;
}
