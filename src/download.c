#include <stdio.h>
#include <unistd.h>
#include "utils.h"
#include "actions.h"
#include "pkg.h"

int
git_download(struct source *s, struct env *e) {
    return 0;
}

int
http_download(struct source *s, struct env *e) {
    if (access(s->cachefile, F_OK) == 0) {
        mylog2("cached", s->cachefile);
        return 0;
    }
    /* TODO: this only works once! need to copy e->get, not modify it. */
    size_t i;
    for (i = 0; i < 6; i++) {
        if (e->get[i] == NULL)
            break;
    }
    if (i > 5) die2(e->get[0], "too many args");
    e->get[i++] = s->cachefile;
    e->get[i++] = s->remote;
    e->get[i] = NULL;

    return run(e->get);
}

int
download(int argc, char **argv, struct env *e) {
    if (argc < 2)
        die2("download", "need a package(s)");

    for (int i = 1; i < argc; i++) {
        struct pkg *p = pkg_parse_sources(argv[i], e);
        for (size_t j = 0; j < p->n; j++) {
            switch (p->s[j]->type) {
                case SRC_GIT:
                    git_download(p->s[j], e);
                    break;
                case SRC_HTTP:
                    http_download(p->s[j], e);
                    break;
                case SRC_FILE: /* FALLTHROUGH */
                default:
                    break;
            }
        }
        pkg_free(p);
    }
    return 0;
}
