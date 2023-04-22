#include <stdio.h>
#include <stdlib.h>
#include "ckiss.h"

noreturn void
usage(int r) {
    mylog("ckiss [l|s|v] [pkg]...");
    mylog("list         List installed packages");
    mylog("search       Search for packages");
    mylog("version      Package manager version");
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
        case 'v':
            printf("%s\n", KISS_VERSION);
            break;
        default:
            /* TODO: external tools */
            usage(1);
    }
    destroy_env(e);
    return 0;
}
