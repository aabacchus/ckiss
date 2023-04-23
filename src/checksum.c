#include <stdio.h>
#include <stdlib.h>

#include "actions.h"
#include "checksums.h"
#include "pkg.h"
#include "utils.h"

int
checksum(int argc, char **argv, struct env *e) {
    int ret = 0;
    if (argc < 2)
        die2("checksum", "need a package name(s)"); /* TODO: crux-like */

    for (int i = 1; i < argc; i++) {
        struct pkg *p = pkg_parse_sources(argv[i], e);
        if (p == NULL || p->n_need_checksums == 0) {
            pkg_free(p);
            mylog2(argv[i], "No sources needing checksums");
            continue;
        }

        FILE *f = pkg_open_file(p->pkg_path, "checksums", "w");
        if (f == NULL)
            die_perror("couldn't open checksums file for writing");

        for (size_t j = 0; j < p->n; j++) {
            char *sum = source_generate_checksum(p->s[j]);
            if (sum) {
                fprintf(f, "%s\n", sum);
                printf("%s\n", sum);
            }
            free(sum);
        }
        fclose(f);
        pkg_free(p);

        mylog2(argv[i], "Generated checksums");
    }
    return ret;
}
