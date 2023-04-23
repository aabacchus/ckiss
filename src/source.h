#ifndef _CKISS_SOURCE_H
#define _CKISS_SOURCE_H

#include "ckiss.h"

enum pkg_source_types {
    SRC_INVAL = 0,
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

/* returns the location of the cache file for the source for pkg, or NULL if not
 * needed (eg git sources or local files in repo) */
char *source_get_cache(char *pkg, char *pkg_path, struct source *s, struct env *e);

enum pkg_source_types pkg_source_type(char *remote, char *pkg_path);

struct source **parse_sources(char *pkg, char *pkg_path, struct env *e);

#endif
