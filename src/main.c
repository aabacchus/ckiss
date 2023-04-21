#include <stdlib.h>
#include "kiss.h"

noreturn void
usage(int r) {
    mylog("kiss [l] [pkg]...");
    mylog("list    List installed packages");
    exit(r);
}

int
main(int argc, char **argv) {
    if (argc < 2) usage(0);

    struct env *e = setup_env();
    switch (argv[1][0]) {
        case 'l':
            list(argc - 1, argv + 1, e);
            break;
        default:
            /* TODO: external tools */
            usage(1);
    }
    destroy_env(e);
    return 0;
}
