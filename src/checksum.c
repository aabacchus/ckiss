#include <blake3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ckiss.h"
#include "source.h"

/* reads f and returns the checksum */
static char *
file_checksum(FILE *f) {
    /* This function is essentially copied from b3sum:
     * https://git.sr.ht/~mcf/b3sum/tree/cb4111ccc8061039b014fbb657c72f78984f1069/item/b3sum.c#L19
     * which is used under CC0. */

    size_t outlen = 33, len;
    unsigned char *out = malloc(outlen);
    if (out == NULL)
        die_perror("malloc");

    size_t buflen = 0x4000;
    char *buf = malloc(buflen);
    if (buf == NULL)
        die_perror("malloc");

    blake3_hasher ctx;
    blake3_hasher_init(&ctx);

    do {
        len = fread(buf, 1, buflen, f);
        if (len > 0)
            blake3_hasher_update(&ctx, buf, len);
    } while (len == buflen);
    if (ferror(f))
        die_perror("fread");

    blake3_hasher_finalize(&ctx, out, outlen);
    free(buf);

    size_t hexlen = 2 * outlen;
    char *hex = malloc(hexlen + 1);
    if (hex == NULL)
        die_perror("malloc");
    for (size_t i = 0; i < outlen; i++)
        sprintf(hex + 2*i, "%02x", out[i]);
    hex[hexlen] = '\0';

    free(out);
    return hex;
}

/* returns the checksum of the file specified by s, if needed and if the cache
 * is present (must download first) */
char *
source_generate_checksum(struct source *s) {
    if (s == NULL || s->type == SRC_INVAL)
        die("source struct not initialised");
    if (s->type != SRC_HTTP && s->type != SRC_FILE)
        return NULL; /* checksum not needed */

    FILE *f = fopen(s->cachefile, "rb");
    if (f == NULL)
        die_perror(s->cachefile);

    char *c = file_checksum(f);
    fclose(f);
    return c;
}

/* returns 1 if all good, 0 if there is a checksum mismatch. */
int
verify_checksums(char *pkg, char *pkg_path, struct source **s) {
    if (s == NULL) {
        mylog2(pkg, "No sources");
        return 1;
    }

    FILE *f = pkg_open_file(pkg_path, "checksums", "r");
    if (f == NULL) {
        /* TODO check if any sources need sums first */
        die_perror("no checksums");
    }

    char *buf = NULL;
    size_t bufn = 0;
    ssize_t n;
    for (int i = 0; s[i] != NULL; i++) {
        if (s[i]->type != SRC_HTTP && s[i]->type != SRC_FILE)
            continue;

        if ((n = getline(&buf, &bufn, f)) == -1) {
            free(buf);
            fclose(f);
            perror(NULL);
            die2(s[i]->remote, "checksums missing");
        }
        if (buf[n - 1] == '\n')
            buf[--n] = '\0';

        char *sum = source_generate_checksum(s[i]);
        if (strcmp(buf, sum) != 0) {
            free(sum);
            free(buf);
            fclose(f);
            mylog2(s[i]->cachefile, "checksum mismatch");
            return 0;
        }
        free(sum);
    }
    free(buf);
    fclose(f);
    return 1;
}

int
checksum(int argc, char **argv, struct env *e) {
    int ret = 0;
    if (argc < 2)
        die2("checksum", "need a package name(s)"); /* TODO: crux-like */

    for (int i = 1; i < argc; i++) {
        bool needed = false;
        char *pkg_path = find_pkg(argv[i], e);
        if (pkg_path == NULL)
            die2(argv[i], "not found");

        FILE *f = pkg_open_file(pkg_path, "checksums", "w");
        if (f == NULL)
            die_perror("couldn't open checksums file for writing");

        struct source **s = parse_sources(argv[i], pkg_path, e);

        for (int j = 0; s[j] != NULL; j++) {
            char *sum = source_generate_checksum(s[j]);
            if (sum) {
                fprintf(f, "%s\n", sum);
                printf("%s\n", sum);
                needed = true;
            }
            free(sum);

            free(s[j]->remote);
            free(s[j]->extract_dir);
            free(s[j]->cachefile);
            free(s[j]);
        }
        free(s);
        fclose(f);
        mylog2(argv[i], needed ? "Generated checksums" : "No sources needing checksums");
    }
    return ret;
}
