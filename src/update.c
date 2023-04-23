#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "utils.h"
#include "actions.h"

/* TODO: use libgit2 */

void
repo_try_print_motd(char *repo) {
    char *fn = concat(repo, "/MOTD", NULL);
    FILE *f = fopen(fn, "r");
    if (f != NULL) {
        int c;
        while ((c = fgetc(f)) != EOF)
            putchar(c);
        fclose(f);
    }
    free(fn);
}

int
update(struct env *e) {
    for (int i = 0; e->kiss_path[i] != NULL; i++) {
        char *repo = e->kiss_path[i];
        mylog(repo);
        /* TODO need to get root of git repo, ideally only pull once each, and use that for motd. */

        /*
        char *buf;
        int n = snprintf(NULL, 0, "git -C \"%s\" pull", repo);
        buf = malloc(n+1);
        if (buf == NULL)
            die_perror("malloc");
        snprintf(buf, n+1, "git -C \"%s\" pull", repo);
        */

        char *cmd[] = {"git", "-C", repo, "pull", NULL};
        if (run(cmd) != 0)
            die_perror(repo);

        repo_try_print_motd(repo);
    }
    return 0;
}
