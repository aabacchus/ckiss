#include <assert.h>
#include <blake3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "checksums.h"

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
    assert(s);
    if (s->type != SRC_HTTP && s->type != SRC_FILE)
        return NULL; /* checksum not needed */

    FILE *f = fopen(s->cachefile, "rb");
    if (f == NULL)
        die_perror(s->cachefile);

    char *c = file_checksum(f);
    fclose(f);
    return c;
}

/* dies if there is a checksum mismatch. */
void
verify_checksums(struct pkg *p) {
    assert(p);
    FILE *f = pkg_open_file(p->pkg_path, "checksums", "r");
    if (f == NULL) {
        if (p->n_need_checksums == 0)
            return;
        else
            die2(p->pkg, "checksums needed but no checksum file");
    }

    assert(p->s);

    char *buf = NULL;
    size_t bufn = 0;
    ssize_t n;
    for (size_t i = 0; i < p->n; i++) {
        if (p->s[i]->type != SRC_HTTP && p->s[i]->type != SRC_FILE)
            continue;

        if ((n = getline(&buf, &bufn, f)) == -1) {
            free(buf);
            fclose(f);
            perror(NULL);
            die2(p->s[i]->remote, "checksums missing");
        }
        if (buf[n - 1] == '\n')
            buf[--n] = '\0';

        char *sum = source_generate_checksum(p->s[i]);
        if (strcmp(buf, sum) != 0) {
            free(sum);
            free(buf);
            fclose(f);
            die2(p->s[i]->cachefile, "checksum mismatch");
        }
        free(sum);
    }
    free(buf);
    fclose(f);
}

