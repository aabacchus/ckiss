#include <stdlib.h>
#include "kiss.h"

char *c1, *c2, *c3;

noreturn void
usage(int r) {
    mylog("kiss [l|s] [pkg]...");
    mylog("list         List installed packages");
    mylog("search       Search for packages");
    exit(r);
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

int
main(int argc, char **argv) {
    struct env *e = setup_env();
    setup_colors(e);

    if (argc < 2) usage(0);

    switch (argv[1][0]) {
        case 'l':
            list(argc - 1, argv + 1, e);
            break;
        case 's':
            search(argc - 1, argv + 1, e);
            break;
        default:
            /* TODO: external tools */
            usage(1);
    }
    destroy_env(e);
    return 0;
}
