#ifndef _CKISS_H
#define _CKISS_H

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef noreturn
#define noreturn
#endif

#define KISS_VERSION "0.1.0 (compat 5.6.4)"

struct env {
    bool	color;
    bool	debug;
    bool	force;
    bool	keeplog;
    bool	prompt;
    char	**hooks;
    char	**kiss_path;
    char	**path;
    char	*cac_dir;
    char	*compress;
    char	*elf;
    char	*get[7];
    char	*pid;
    char	*pwd;
    char	*root;
    char	*su;
    char	*sys_db;
    char	*tmpdir;
    char	date[17]; /* YYYY-MM-DD-HH:MM + '\0' */
};

/* include these now in case they need struct env */
#include "pkg.h"

/* returns the checksum of the file specified by s, if needed and if the cache
 * is present (must download first) */
char *source_generate_checksum(struct source *s);

/* dies if there is a checksum mismatch. */
void verify_checksums(struct pkg *p);

int list(int argc, char **argv, struct env *e);
int search(int argc, char **argv, struct env *e);
int checksum(int argc, char **argv, struct env *e);

#endif
