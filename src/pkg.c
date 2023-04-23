#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "pkg.h"
#include "utils.h"

/* returns the location of the cache file for the source for pkg, or NULL if not
 * needed (eg git sources or local files in repo) */
static char *
source_get_cache(char *pkg, char *pkg_path, struct source *s, struct env *e) {
    assert(s);

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

struct pkg *
pkg_parse_sources(char *pkg, struct env *e) {
    struct source **s = NULL;
    char *pkg_path = find_pkg_path(pkg, e);
    if (pkg_path == NULL)
        die2(pkg, "not found");

    mylog2(pkg, "Reading sources");

    FILE *f = pkg_open_file(pkg_path, "sources", "r");

    if (f == NULL) {
        free(pkg_path);
        if (errno == ENOENT) {
            mylog2(pkg, "no sources file, skipping");
            return NULL;
        } else {
            die2(pkg, "couldn't open sources file");
        }
    }

    char *buf = NULL;
    size_t bufn = 0;
    ssize_t n;
    int lineno = 0, needed = 0;
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

        if (new->type == SRC_FILE || new->type == SRC_HTTP)
            needed++;

        /* add new to the list */
        s = realloc(s, sizeof(struct source *) * lineno);
        if (s == NULL)
            die_perror("realloc");
        s[lineno-1] = new;
    }
    fclose(f);
    free(buf);

    struct pkg *p = calloc(1, sizeof(struct pkg));
    if (p == NULL)
        die_perror("calloc");

    p->pkg = pkg;
    p->pkg_path = pkg_path;
    p->n = lineno;
    p->n_need_checksums = needed;
    p->s = s;

    return p;
}

void
pkg_free(struct pkg *p) {
    if (p == NULL)
        return;
    for (size_t i = 0; i < p->n; i++) {
        free(p->s[i]->remote);
        free(p->s[i]->extract_dir);
        free(p->s[i]->cachefile);
        free(p->s[i]);
    }
    free(p->s);
    free(p->pkg_path);
    free(p);
}

char *
find_pkg_path(char *name, struct env *e) {
    array_t full_path = arr_copy(e->kiss_path);
    arr_append(&full_path, e->sys_db, -1, true);

    array_t s = find_in_path(name, full_path, S_IFDIR, true, false);
    char *pkg = NULL;
    if (s != NULL)
        pkg = s[0];

    free(s);
    arr_free(full_path);
    return pkg;
}

FILE *
pkg_open_file(char *pkg_path, char *file, char *mode) {
    char *s = concat(pkg_path, "/", file, NULL);
    FILE *f = fopen(s, mode);
    if (f == NULL)
        return NULL;
    free(s);
    return f;
}
