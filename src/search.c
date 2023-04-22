#include <stdio.h>
#include <stdlib.h>
#include "ckiss.h"

int
search(int argc, char **argv, struct env *e) {
    /* make a copy of kiss_path and append sys_db to it */
    char **p = arr_copy(e->kiss_path);
    append_to_array(&p, e->sys_db, -1, true);

    for (int i = 1; i < argc; i++) {
        char **s = find_in_path(argv[i], p, S_IFDIR, false, true);

        if (s == NULL)
            die2(argv[i], "not found");

        for (int j = 0; s[j] != NULL; j++) {
            printf("%s\n", s[j]);
            free(s[j]);
        }

        free(s);
    }
    return 0;
}
