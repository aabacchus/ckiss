#include <stdio.h>
#include <stdlib.h>
#include "ckiss.h"

noreturn void
usage(int r) {
    mylog("ckiss [c|l|s|v] [pkg]...");
    mylog("checksum     Generate checksums");
    mylog("list         List installed packages");
    mylog("search       Search for packages");
    mylog("version      Package manager version");
    exit(r);
}

int
main(int argc, char **argv) {
    int ret = 0;
    struct env *e = setup_env();
    setup_colors(e);

    if (argc < 2) usage(0);

    switch (argv[1][0]) {
        case 'c':
            ret = checksum(argc - 1, argv + 1, e);
            break;
        case 'l':
            ret = list(argc - 1, argv + 1, e);
            break;
        case 's':
            ret = search(argc - 1, argv + 1, e);
            break;
        case 'v':
            printf("%s\n", KISS_VERSION);
            break;
        default:
            /* TODO: external tools */
            usage(1);
    }
    destroy_env(e);
    return ret;
}
