#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ckiss.h"
#include "source.h"

char *
source_get_cache(char *pkg, char *pkg_path, struct source *s, struct env *e) {
    if (s == NULL || s->type == SRC_INVAL)
        die("source struct not initialised");

    switch (s->type) {
        case SRC_HTTP:
            {
                char *filepart = strrchr(s->remote, '/');
                if (filepart == NULL)
                    die2(s->remote, "filepart of URL not found");
                filepart++;
                char *cache = concat(e->cac_dir, "/sources/", pkg, "/",
                        s->extract_dir ? s->extract_dir : "", filepart, NULL);
                return cache;
            }
        case SRC_FILE:
            {
                char *f = concat(pkg_path, "/", s->remote, NULL);
                return f;
            }
        default:
            return NULL;
    }
}

enum pkg_source_types
pkg_source_type(char *remote, char *pkg_path) {
    if (strncmp("git+", remote, 4) == 0)
        return SRC_GIT;
    if (strncmp("http", remote, 4) == 0)
        return SRC_HTTP;
    if (strstr(remote, "://"))
        die2(remote, "protocol not supported");

    /* XXX: only relative files are supported. */
    char *f = concat(pkg_path, "/", remote, NULL);
    int ok = (access(f, F_OK) == 0);
    free(f);
    if (ok)
        return SRC_FILE;

    die2(remote, "invalid source");
}

struct source **
parse_sources(char *pkg, char *pkg_path, struct env *e) {
    struct source **s = NULL;

    mylog2(pkg, "Reading sources");

    FILE *f = pkg_open_file(pkg_path, "sources", "r");

    if (f == NULL) {
        if (errno == ENOENT) {
            mylog2(pkg, "no sources file, skipping");
            goto sources_ret;
        } else {
            die2(pkg, "couldn't open sources file");
        }
    }

    char *buf = NULL;
    size_t bufn = 0;
    ssize_t n;
    int lineno = 0;
    while ((n = getline(&buf, &bufn, f)) != -1) {
        if (n == 0 || buf[0] == '#' || buf[0] == '\n')
            continue;

        lineno++;

        struct source *new = calloc(1, sizeof(struct source));
        if (new == NULL) die_perror("calloc");

        array_t t = split(buf, " \t\n");

        new->remote = t[0];
        new->extract_dir = t[1];
        free(t); /* just free t, not the strings in it */

        new->type = pkg_source_type(new->remote, pkg_path);
        new->cachefile = source_get_cache(pkg, pkg_path, new, e);

        /* add new to the list */
        s = realloc(s, sizeof(struct source *) * (lineno + 1));
        if (s == NULL)
            die_perror("realloc");
        s[lineno-1] = new;

        /* add a final NULL */
        s[lineno] = NULL;
    }
    fclose(f);
    free(buf);

sources_ret:
    free(pkg_path);
    return s;
}
