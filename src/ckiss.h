#include <limits.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdnoreturn.h>
#include <sys/stat.h>

#ifndef noreturn
#define noreturn
#endif

#define KISS_VERSION "0 (compat 5.6.4)"

struct cmd {
    char **args;
};

struct env {
    bool	color;
    bool	debug;
    bool	force;
    bool	keeplog;
    bool	prompt;
    char	**hooks;
    char	**kiss_path;
    char	**path;
    char	*b3[5];
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

/* Appends to *arr (allocs if NULL). If you don't know the length of arr but it
 * has a terminating NULL, supply an n < 0. If s should be strdup'd then set dup
 * to true. */
char **append_to_array(char ***arr, char *s, int n, bool dup);

/* splits s by any delimiters in sep into an array of strings. Each string and
 * the array must be freed by the caller. */
char **split(char *s, char *sep);

/* Goes through the path array looking for a file in each directory with the
 * given name. Returns the first one. The returned string must be freed by the
 * caller. If limit is true, only return the first result found in path,
 * otherwise return an array of all results found in path. If isglob is true,
 * treat name as a glob. test_flags is OR'd with the file's mode (from stat(3))
 * to check if it matches a criterion (eg. executable, directory). */
char **find_in_path(char *name, char **path, mode_t test_flags, bool limit, bool isglob);

/* Checks for the first cmd which may be found in path. Returns the index of the
 * cmd (0, 1, 2, ...). Arg list must be terminated with a NULL */
int available_cmd(char **path, char *cmd, ...);

/* setup internal colours used by the logging functions. */
void setup_colors(struct env *e);

/* parses environment variables to initialise a struct env. */
struct env *setup_env(void);

/* correctly frees a struct env. */
void destroy_env(struct env *e);


int list(int argc, char **argv, struct env *e);
int search(int argc, char **argv, struct env *e);
