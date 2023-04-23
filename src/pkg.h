#ifndef _CKISS_SOURCE_H
#define _CKISS_SOURCE_H

#include "ckiss.h"

enum pkg_source_types {
    SRC_HTTP,
    SRC_GIT,
    SRC_FILE,
};

struct source {
    enum pkg_source_types type;
    char *remote; /* location of file to be downloaded/copied */
    char *extract_dir; /* optional dir to extract file into */
    char *cachefile; /* cache location if file downloaded */
};

struct pkg {
    char *pkg;
    char *pkg_path;
    size_t n;
    size_t n_need_checksums;
    struct source **s;
};

enum pkg_source_types pkg_source_type(char *remote, char *pkg_path);

struct pkg *pkg_parse_sources(char *pkg, struct env *e);

void pkg_free(struct pkg *p);

/* Wrapper around find_in_path. Name is an exact name, not a glob. Return the
 * first package in $KISS_PATH:$sys_db, or NULL. */
char *find_pkg_path(char *name, struct env *e);

FILE *pkg_open_file(char *pkg_path, char *file, char *mode);

#endif
