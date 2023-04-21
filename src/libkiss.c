#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kiss.h"

void
mylog(const char *s) {
    fprintf(stderr, "-> %s\n", s);
}

void
warn(const char *s) {
    fprintf(stderr, "WARNING %s\n", s);
}

void
die(const char *s) {
    mylog(s);
    exit(1);
}

void
die_perror(const char *s) {
    perror(s);
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

char **
split(char *s, char *sep) {
    if (s == NULL)
        return NULL;

    char **res = NULL, **tmp;
    char *p = strtok(s, sep);
    int n = 0;

    while (p) {
        tmp = realloc(res, sizeof(char *) * ++n);
        if (tmp == NULL) {
            free(res);
            die_perror("realloc");
        }
        res = tmp;
        res[n - 1] = strdup(p);
        p = strtok(NULL, sep);
    }

    /* add a NULL terminator */
    tmp = realloc(res, sizeof(char *) * (n + 1));
    if (tmp == NULL) {
        free(res);
        die_perror("realloc");
    }
    res = tmp;
    res[n] = NULL;

    return res;
}

char *
find_in_path(char *name, char **path) {
    for (int i = 0; path[i] != NULL; i++) {
        char *file = concat(path[i], "/", name, NULL);
        if (access(file, R_OK) == 0) {
            return file;
        }
        free(file);
    }
    return NULL;
}

int
available_cmd(char **path, char *cmd, ...) {
    char *s;
    va_list ap;
    int n = 0;
    va_start(ap, cmd);
    while (cmd != NULL) {
        s = find_in_path(cmd, path);
        if (s != NULL) {
            free(s);
            va_end(ap);
            return n;
        }
        free(s);
        n++;
        cmd = va_arg(ap, char *);
    }
    va_end(ap);
    return -1;
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

    switch (available_cmd(e->path, "b3sum", NULL)) {
        case 0:
            e->b3[0] = "b3sum";
            e->b3[1] = "-l";
            e->b3[2] = "33";
            e->b3[4] = NULL;
            break;
        default:
            die("b3sum utility not found");
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
    for (int i = 0; e->hooks[i] != NULL; i++)
        free(e->hooks[i]);
    free(e->hooks);

    for (int i = 0; e->kiss_path[i] != NULL; i++)
        free(e->kiss_path[i]);
    free(e->kiss_path);

    for (int i = 0; e->path[i] != NULL; i++)
        free(e->path[i]);
    free(e->path);

    free(e->sys_db);
    free(e->pid);
    free(e);
}
