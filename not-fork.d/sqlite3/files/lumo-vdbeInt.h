#ifndef _LUMO_VDBEINT_
#define _LUMO_VDBEINT_ 1

#ifdef LUMO_ROWSUM

/* declarations needed for the rowsum code */

#include "lumo-sha3.c"

#define LUMO_EXTENSIONS 1
#define LUMO_ROWSUM_TYPE 1

#define LUMO_ROWSUM_ID_none 65535
#define LUMO_ROWSUM_ID_null 0
#define LUMO_ROWSUM_ID_sha3_512 1
#define LUMO_ROWSUM_ID_sha3_384 2
#define LUMO_ROWSUM_ID_sha3_256 3
#define LUMO_ROWSUM_ID_sha3_224 4
#define LUMO_ROWSUM_ID_sha3 LUMO_ROWSUM_ID_sha3_256

#define LUMO_ROWSUM_DECLARATIONS_sha3(k) \
  static void lumo_init_sha3_##k(void * ctx) { \
    SHA3Init(ctx, k); \
  } \
  static void lumo_update_sha3_##k(void * ctx, const void * p, unsigned int n) { \
    SHA3Update(ctx, p, n); \
  } \
  static void lumo_final_sha3_##k(void * ctx, unsigned char * d) { \
    memcpy(d, SHA3Final(ctx), k / 8); \
  } \
  static void lumo_generate_sha3_##k(unsigned char * d, const void * p, unsigned int n) { \
    SHA3Context ctx; \
    SHA3Init(&ctx, k); \
    SHA3Update(&ctx, p, n); \
    memcpy(d, SHA3Final(&ctx), k / 8); \
  }

LUMO_ROWSUM_DECLARATIONS_sha3(512)
LUMO_ROWSUM_DECLARATIONS_sha3(384)
LUMO_ROWSUM_DECLARATIONS_sha3(256)
LUMO_ROWSUM_DECLARATIONS_sha3(224)

#undef LUMO_ROWSUM_DECLARATIONS_sha3

#define LUMO_ROWSUM_ELEMENT_sha3(k) \
    [LUMO_ROWSUM_ID_sha3_##k] = { \
	"sha3_" #k, \
	k / 8, \
	lumo_generate_sha3_##k, \
	sizeof(SHA3Context), \
	lumo_init_sha3_##k, \
	lumo_update_sha3_##k, \
	lumo_final_sha3_##k, \
    },

static struct {
    const char * name;
    int length;
    void (*generate)(unsigned char *, const void *, unsigned int);
    size_t mem;
    void (*init)(void *);
    void (*update)(void *, const void *, unsigned int);
    void (*final)(void *, unsigned char *);
} lumo_rowsum_algorithms[] = {
    [LUMO_ROWSUM_ID_null] = { "empty", 0, NULL, 0, NULL, NULL, NULL },
    LUMO_ROWSUM_ELEMENT_sha3(512)
    LUMO_ROWSUM_ELEMENT_sha3(384)
    LUMO_ROWSUM_ELEMENT_sha3(256)
    LUMO_ROWSUM_ELEMENT_sha3(224)
};
#define LUMO_ROWSUM_N_ALGORITHMS \
    (sizeof(lumo_rowsum_algorithms) / sizeof(lumo_rowsum_algorithms[0]))

#undef LUMO_ROWSUM_ELEMENT_sha3

static struct {
    const char * name;
    unsigned int same_as;
} lumo_rowsum_alias[] = {
    { "sha3",   LUMO_ROWSUM_ID_sha3_256 },
    { "none",   LUMO_ROWSUM_ID_none },
};
#define LUMO_ROWSUM_N_ALIAS \
    (sizeof(lumo_rowsum_alias) / sizeof(lumo_rowsum_alias[0]))

/* default value when creating a new table */
extern unsigned int lumo_rowsum_algorithm;

/* how we check the rowsum: 0, we don't check it at all; 1, we check it
** if present, but don't require it; 2, we require it to be there and
** check it */
extern int lumo_extension_check_rowsum;
#endif

#ifdef LUMO_EXTENSIONS
#define LUMO_END_TYPE 0

/* help making sure we only look at our data */
#define LUMO_EXTENSION_MAGIC "Lumo"
#endif

#endif
