#ifndef _CKISS_CHECKSUMS_H
#define _CKISS_CHECKSUMS_H
#include "pkg.h"

/* returns the checksum of the file specified by s, if needed and if the cache
 * is present (must download first) */
char *source_generate_checksum(struct source *s);

/* dies if there is a checksum mismatch. */
void verify_checksums(struct pkg *p);
#endif
