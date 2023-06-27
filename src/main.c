#include <stdio.h>
#include <stdlib.h>

#include "actions.h"
#include "utils.h"

#define KISS_VERSION "0.1.0 (compat 5.6.4)"

noreturn void
usage(int r) {
    mylog("ckiss [c|l|s|v] [pkg]...");
    mylog("checksum     Generate checksums");
    mylog("download     Download sources");
    mylog("list         List installed packages");
    mylog("search       Search for packages");
    mylog("update       Update the repositories");
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
        case 'd':
            ret = download(argc - 1, argv + 1, e);
            break;
        case 'l':
            ret = list(argc - 1, argv + 1, e);
            break;
        case 's':
            ret = search(argc - 1, argv + 1, e);
            break;
        case 'u':
            ret = update(e);
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
