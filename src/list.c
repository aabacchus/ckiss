#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kiss.h"

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

    free(p);
    fclose(f);
}

int
list(int argc, char **argv, struct env *e) {
    struct dirent *dp;

    if (argc == 1) {
        DIR *d = opendir(e->sys_db);
        if (d == NULL) die_perror(e->sys_db);
        do {
            errno = 0;
            dp = readdir(d);
            if (dp == NULL) {
                if (errno == 0) {
                    /* end of dir */
                    goto done;
                }
                closedir(d);
                die_perror("readdir");
            }
            if (dp->d_name[0] == '.')
                continue;
            pkg_print(dp->d_name, e);
        } while (dp != NULL);
done:
        closedir(d);
    } else {
        for (int i = 1; i < argc; i++) {
            pkg_print(argv[i], e);
        }
    }
    return 0;
}
