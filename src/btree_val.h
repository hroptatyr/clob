/*** btree_val.h -- values and keys of btrees ***/
#pragma once
#include <stdbool.h>
#include "plqu.h"
#include "plqu_val.h"

/* keys are prices */
typedef _Decimal64 btree_key_t;

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

