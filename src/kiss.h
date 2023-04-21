#include <limits.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdnoreturn.h>

#ifndef noreturn
#define noreturn
#endif

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
void warn(const char *s);
noreturn void die(const char *s);
noreturn void die_perror(const char *s);

/* returns a string containing the concatenation of all args. Args must be
 * terminated with a NULL. Returned string must be freed by caller.*/
char *concat(char *s, ...);

/* splits s by any delimiters in sep into an array of strings. Each string and
 * the array must be freed by the caller. */
char **split(char *s, char *sep);

/* Goes through the path array looking for a file in each directory with the
 * given name. Returns the first one. The returned string must be freed by the
 * caller. */
char *find_in_path(char *name, char **path);

/* Checks for the first cmd which may be found in path. Returns the index of the
 * cmd (0, 1, 2, ...). Arg list must be terminated with a NULL */
int available_cmd(char **path, char *cmd, ...);

/* parses environment variables to initialise a struct env. */
struct env *setup_env(void);

/* correctly frees a struct env. */
void destroy_env(struct env *e);


int list(int argc, char **argv, struct env *e);
