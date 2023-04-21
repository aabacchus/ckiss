#include <stdio.h>
#include <stdlib.h>
#include "kiss.h"

int
search(int argc, char **argv, struct env *e) {
    for (int i = 1; i < argc; i++) {
        char **s = find_in_path(argv[i], e->kiss_path, false);
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
