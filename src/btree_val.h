/*** btree_val.h -- values and keys of btrees ***/
#if !defined INCLUDED_btree_val_h_
#define INCLUDED_btree_val_h_
#include <stdbool.h>
#include "clob_val.h"
#include "plqu.h"

/* keys are prices */
typedef px_t btree_key_t;

/* values are plqu's and a plqu_val_t for the sum */
typedef struct {
	plqu_t q;
	qty_t sum;
} btree_val_t;

#define btree_val_nil	((btree_val_t){0U})

static inline bool
btree_val_nil_p(btree_val_t v)
{
	return v.q == 0U;
}

static inline void
free_btree_val(btree_val_t v)
{
	free_plqu(v.q);
	return;
}

#endif	/* INCLUDED_btree_val_h_ */
