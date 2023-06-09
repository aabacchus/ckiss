#ifndef _CKISS_UTILS_H
#define _CKISS_UTILS_H

#include <stdbool.h>
#include <stdnoreturn.h>
#include <sys/stat.h>

#include "array.h"

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


/* called "mylog" to avoid collision with math.h log function. */
void mylog(const char *s);
void mylog2(const char *name, const char *s);
void mylog_v(char *format, ...);
void warn(const char *s);
void warn2(const char *name, const char *s);
noreturn void die(const char *s);
noreturn void die2(const char *name, const char *s);
noreturn void die_perror(const char *s);

/* returns a string containing the concatenation of all args. Args must be
 * terminated with a NULL. Returned string must be freed by caller.*/
char *concat(char *s, ...);

/* Goes through the path array looking for a file in each directory with the
 * given name. Returns the first one. The returned string must be freed by the
 * caller. If limit is true, only return the first result found in path,
 * otherwise return an array of all results found in path. If isglob is true,
 * treat name as a glob. test_flags is OR'd with the file's mode (from stat(3))
 * to check if it matches a criterion (eg. executable, directory). */
array_t find_in_path(char *name, array_t path, mode_t test_flags, bool limit, bool isglob);

/* Checks for the first cmd which may be found in path. Returns the index of the
 * cmd (0, 1, 2, ...). Arg list must be terminated with a NULL */
int available_cmd(array_t path, char *cmd, ...);

/* setup internal colours used by the logging functions. */
void setup_colors(struct env *e);

/* parses environment variables to initialise a struct env. */
struct env *setup_env(void);

/* correctly frees a struct env. */
void destroy_env(struct env *e);

int run(char *argv[]);
#endif
