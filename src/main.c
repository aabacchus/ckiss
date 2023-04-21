#include <stdlib.h>
#include "kiss.h"

noreturn void
usage(int r) {
    mylog("kiss [l|s] [pkg]...");
    mylog("list         List installed packages");
    mylog("search       Search for packages");
    exit(r);
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
