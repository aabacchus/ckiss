#include <errno.h>
#include <glob.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "array.h"
#include "utils.h"

static char *c1 = "", *c2 = "", *c3 = "";

void
mylog(const char *s) {
    fprintf(stderr, "%s-> %s%s\n", c1, c3, s);
}

void
mylog2(const char *name, const char *s) {
    fprintf(stderr, "%s-> %s%s%s %s\n", c1, c2, name, c3, s);
}

void
warn(const char *s) {
    fprintf(stderr, "%sWARNING %s%s\n", c1, c3, s);
}

void
warn2(const char *name, const char *s) {
    fprintf(stderr, "%sWARNING %s%s%s %s\n", c1, c2, name, c3, s);
}

void
die(const char *s) {
    fprintf(stderr, "%sERROR %s%s\n", c1, c3, s);
    exit(1);
}

void
mylog_v(char *format, ...) {
    va_list ap;
    va_start(ap, format);
    fprintf(stderr, "%s-> %s", c1, c3);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

void
die2(const char *name, const char *s) {
    fprintf(stderr, "%sERROR %s%s%s %s\n", c1, c2, name, c3, s);
    exit(1);
}

void
die_perror(const char *s) {
    die2(s, strerror(errno));
    exit(1);
}

char *
concat(char *s, ...) {
    char *c, *tmp;
    size_t len = 0;
    va_list ap;

    /* find total length of string */
    va_start(ap, s);
    tmp = s;
    while (tmp != NULL) {
        len += strlen(tmp);
        tmp = va_arg(ap, char *);
    }
    len += 1; /* terminating NUL */
    va_end(ap);

    c = calloc(len, sizeof(char));
    if (c == NULL) die_perror("calloc");

    /* now concatenate */
    va_start(ap, s);
    tmp = s;
    strcpy(c, tmp);
    tmp = va_arg(ap, char *);
    while (tmp != NULL) {
        strcat(c, tmp);
        tmp = va_arg(ap, char *);
    }
    va_end(ap);
    c[len-1] = '\0';
    return c;
}

array_t
find_in_path(char *name, array_t path, mode_t test_flags, bool limit, bool isglob) {
    array_t s = NULL;
    int n = 0;
    if (path == NULL || name == NULL)
	    return s;
    for (int i = 0; path[i] != NULL; i++) {
        char *file = concat(path[i], "/", name, NULL);
        array_t list = NULL;
        if (isglob) {
            glob_t pglob;
            int r = glob(file, GLOB_ERR, NULL, &pglob);
            free(file);
            if (r != 0 || pglob.gl_pathc == 0) {
                if (r != GLOB_NOMATCH)
                    die_perror("glob");
                continue;
            }
            list = malloc(sizeof(char *) * (pglob.gl_pathc + 1));
            if (list == NULL) die_perror("malloc");
            for (size_t j = 0; j < pglob.gl_pathc; j++) {
                list[j] = strdup(pglob.gl_pathv[j]);
	    }
	    list[pglob.gl_pathc] = NULL;
            globfree(&pglob);
        } else {
            /* just put the one file into an array like pglob.gl_pathv so that
             * the code below can be used for both cases of isglob. */
            list = malloc(sizeof(char *) * 2);
            if (list == NULL) die_perror("malloc");
            list[0] = file;
            list[1] = NULL;
        }

        for (int j = 0; list[j] != NULL; j++) {
            struct stat sb;
            if (stat(list[j], &sb) == -1 || !(sb.st_mode & test_flags)) {
                free(list[j]);
            } else {
                char *found = list[j];
                arr_append(&s, found, n++, false);
                if (limit) {
                    free(list);
                    arr_append(&s, NULL, n, false);
                    return s;
                }
            }
        }
        free(list);
    }
    if (s != NULL) {
        /* add terminating NULL */
        arr_append(&s, NULL, n, false);
    }
    return s;
}

int
available_cmd(array_t path, char *cmd, ...) {
    va_list ap;
    int n = 0;
    va_start(ap, cmd);
    while (cmd != NULL) {
        array_t s = find_in_path(cmd, path, S_IXUSR | S_IXGRP | S_IXOTH, true, false);
        if (s != NULL) {
            free(*s);
            free(s);
            va_end(ap);
            return n;
        }
        n++;
        cmd = va_arg(ap, char *);
    }
    va_end(ap);
    return -1;
}

void
setup_colors(struct env *e) {
    if (!isatty(1) || e->color == 0) {
        c1 = "";
        c2 = "";
        c3 = "";
    } else {
        c1 = "\033[1;33m";
        c2 = "\033[1;34m";
        c3 = "\033[m";
    }
}

struct env *
setup_env(void) {
    char *t;
    struct env *e = calloc(1, sizeof(struct env));
    if (e == NULL) die_perror("calloc");

    t = getenv("KISS_COLOR");
    e->color = !(t && *t == '0');

    t = getenv("KISS_DEBUG");
    e->debug = (t && *t == '1');

    t = getenv("KISS_FORCE");
    e->force = (t && *t == '1');

    t = getenv("KISS_KEEPLOG");
    e->keeplog = (t && *t == '1');

    t = getenv("KISS_PROMPT");
    e->prompt = !(t && *t == '0');

    t = getenv("KISS_HOOK");
    e->hooks = split(t, ":");

    t = getenv("KISS_PATH");
    e->kiss_path = split(t, ":");

    t = getenv("PATH");
    e->path = split(t, ":");

    t = getenv("PWD");
    if (t == NULL) die("PWD is not set");
    e->pwd = t;

    t = getenv("KISS_ROOT");
    e->root = t ? t : "";

    e->sys_db = concat(e->root, "/var/db/kiss/installed", NULL);

    t = getenv("XDG_CACHE_HOME");
    if (t == NULL || *t == '\0') {
        t = getenv("HOME");
        e->cac_dir = concat(t, "/.cache/kiss", NULL);
    } else {
        e->cac_dir = concat(t, "/kiss", NULL);
    }

    time_t dt = time(NULL);
    struct tm *tm = localtime(&dt);
    if (tm == NULL) die_perror("localtime");
    strftime(e->date, sizeof(e->date), "%Y-%m-%d-%H:%M", tm);

    e->tmpdir = getenv("KISS_TMPDIR"); /* TODO */

    t = getenv("KISS_PID");
    if (t && *t != '\0') {
        e->pid = strdup(t);
    } else {
        pid_t p = getpid();
        int n = snprintf(NULL, 0, "%d", p);
        if (n < 0) die_perror("snprintf");
        e->pid = malloc(n+1);
        if (e->pid == NULL) die_perror("malloc");
        snprintf(e->pid, n, "%d", p);
    }

    t = getenv("KISS_COMPRESS");
    e->compress = (t && *t != '\0') ? t : "gz";

    switch (available_cmd(e->path, "aria2c", "axel", "curl", "wget", "wget2", NULL)) {
        case 0:
            e->get[0] = "aria2c";
            e->get[1] = "-d";
            e->get[2] = "/";
            e->get[3] = "-o";
            e->get[4] = NULL;
            break;
        case 1:
            e->get[0] = "axel";
            e->get[1] = "-o";
            e->get[2] = NULL;
            break;
        case 2:
            e->get[0] = "curl";
            e->get[1] = "-fLo";
            e->get[2] = NULL;
            break;
        case 3:
            e->get[0] = "wget";
            e->get[1] = "-O";
            e->get[2] = NULL;
            break;
        case 4:
            e->get[0] = "wget2";
            e->get[1] = "-O";
            e->get[2] = NULL;
            break;
        default:
            die("No download utility found (aria2c, axel, curl, wget, wget2)");
    }

    /* su args are handled in as_user because 'su' needs a different order to
     * the others */
    t = getenv("KISS_SU");
    if (t && *t != '\0') {
        e->su = t;
    } else {
        switch (available_cmd(e->path, "ssu", "sudo", "doas", "su", NULL)) {
            case 0:
                e->su = "ssu";
                break;
            case 1:
                e->su = "sudo";
                break;
            case 2:
                e->su = "doas";
                break;
            case 3:
                e->su = "su";
                break;
            default:
                die("No su utility found (ssu, sudo, doas, su)");
        }
    }

    t = getenv("KISS_ELF");
    if (t && *t != '\0') {
        e->elf = t;
    } else {
        switch (available_cmd(e->path, "readelf", "eu-readelf", "llvm-readelf", "ldd", NULL)) {
            case 0:
                e->elf = "readelf";
                break;
            case 1:
                e->elf = "eu-readelf";
                break;
            case 2:
                e->elf = "llvm-readelf";
                break;
            case 3:
                e->elf = "ldd";
                break;
            default:
                e->elf = NULL;
                warn("No readelf utility found (readelf, eu-readelf, llvm-readelf, ldd)");
        }
    }

    return e;
}

void
destroy_env(struct env *e) {
    if (e == NULL)
        return;
    for (int i = 0; e->hooks && e->hooks[i] != NULL; i++)
        free(e->hooks[i]);
    free(e->hooks);

    for (int i = 0; e->kiss_path && e->kiss_path[i] != NULL; i++)
        free(e->kiss_path[i]);
    free(e->kiss_path);

    for (int i = 0; e->path && e->path[i] != NULL; i++)
        free(e->path[i]);
    free(e->path);

    free(e->sys_db);
    free(e->cac_dir);
    free(e->pid);
    free(e);
}

int
run(char *argv[]) {
    char *sep = "";
    for (int i = 0; argv[i] != NULL; i++) {
        fprintf(stderr, "%s%s", sep, argv[i]);
        sep = " ";
    }
    fprintf(stderr, "\n");

    pid_t pid = fork();
    int stat_loc;

    switch (pid) {
        case -1:
            die_perror("fork");
            return 1;
        case 0:
            execvp(argv[0], argv);

            break;
        default:
            waitpid(pid, &stat_loc, 0);
            return WEXITSTATUS(stat_loc);
    }
    return 0;
}
