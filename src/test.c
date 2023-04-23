#include <stdio.h>
#include "ckiss.h"

int
main(int argc, char **argv) {
    struct env *e = setup_env();

    printf("%s\t%d\n", "color", e->color);
    printf("%s\t%d\n", "debug", e->debug);
    printf("%s\t%d\n", "force", e->force);
    printf("%s\t%d\n", "keeplog", e->keeplog);
    printf("%s\t%d\n", "prompt", e->prompt);

    printf("hooks:\t");
    for (int i = 0; e->hooks[i] != NULL; i++)
        printf("%s, ", e->hooks[i]);
    printf("\n");

    printf("kiss_path:\t");
    for (int i = 0; e->kiss_path[i] != NULL; i++)
        printf("%s, ", e->kiss_path[i]);
    printf("\n");

    printf("path:\t");
    for (int i = 0; e->path[i] != NULL; i++)
        printf("%s, ", e->path[i]);
    printf("\n");

    printf("%s\t%s\n", "compress", e->compress);
    printf("%s\t%s\n", "elf", e->elf);

    printf("get:\t");
    for (int i = 0; e->get[i] != NULL; i++)
        printf("%s, ", e->get[i]);
    printf("\n");

    printf("%s\t%s\n", "pwd", e->pwd);
    printf("%s\t%s\n", "root", e->root);
    printf("%s\t%s\n", "su", e->su);
    printf("%s\t%s\n", "tmpdir", e->tmpdir);
    printf("%s\t%s\n", "date", e->date);
    printf("%s\t%s\n", "pid", e->pid);

    destroy_env(e);
    return 0;
}
