#include <limits.h>
#include <stdbool.h>
#include <unistd.h>

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
    char	*tmpdir;
    char	date[17]; /* YYYY-MM-DD-HH:MM + '\0' */
};

void mylog(const char *s);
void warn(const char *s);
void die(const char *s);
void die_perror(const char *s);

char **split(char *s, char *sep);
char *find_in_path(char *name, char **path);
int available_cmd(char **path, char *cmd, ...);

struct env *setup_env(void);
void destroy_env(struct env *e);
