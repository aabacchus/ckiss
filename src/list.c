#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ckiss.h"
#include "utils.h"

void
pkg_print(char *pkg, struct env *e) {
    char *p = concat(e->sys_db, "/", pkg, "/version", NULL);
    char *buf = NULL;
    size_t bufn = 0;
    FILE *f = fopen(p, "r");
    if (f == NULL) {
        die2(pkg, "not found");
    }

    if (getline(&buf, &bufn, f) == -1) die_perror("getline");
    char *sp = strchr(buf, ' ');
    if (sp) *sp = '-';

    printf("%s %s", pkg, buf);

    free(buf);
    free(p);
    fclose(f);
}

/* exclude .* from package listing */
static int
sel(const struct dirent *d) {
    struct dirent *e = (struct dirent *)d;
    return e->d_name[0] != '.';
}

int
list(int argc, char **argv, struct env *e) {
    if (argc == 1) {
        struct dirent **namelist;
        int n = scandir(e->sys_db, &namelist, sel, alphasort);
        if (n == -1)
            die_perror("scandir");

        for (int i = 0; i < n; i++) {
            pkg_print(namelist[i]->d_name, e);
            free(namelist[i]);
        }

        free(namelist);
        return 0;
    } else {
        for (int i = 1; i < argc; i++) {
            pkg_print(argv[i], e);
        }
    }
    return 0;
}
