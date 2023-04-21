#include <stdio.h>
#include <stdlib.h>
#include "kiss.h"

int
search(int argc, char **argv, struct env *e) {
    char **s = find_in_path(argv[1], e->kiss_path, false);
    if (s == NULL)
        die2(argv[1], "not found");
    for (int i = 0; s[i] != NULL; i++) {
        printf("%s\n", s[i]);
        free(s[i]);
    }
    free(s);
    return 0;
}
