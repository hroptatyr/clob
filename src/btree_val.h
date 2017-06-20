/*** btree_val.h -- values and keys of btrees ***/
#if !defined INCLUDED_btree_val_h_
#define INCLUDED_btree_val_h_
#include <stdbool.h>
#include "clob_val.h"
#include "plqu.h"
#include "plqu_val.h"

/* keys are prices */
typedef px_t btree_key_t;

/* values are plqu's and a plqu_val_t for the sum */
typedef struct {
	plqu_t q;
	plqu_val_t sum;
} btree_val_t;

#define btree_val_nil	((btree_val_t){NULL})

static inline bool
btree_val_nil_p(btree_val_t v)
{
	return v.q == NULL;
}

#endif	/* INCLUDED_btree_val_h_ */
