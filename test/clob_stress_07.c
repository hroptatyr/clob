#include <stdio.h>
#include "dfp754_d64.h"
#include "clob.h"
#include "btree.h"
#include "btree_val.h"
#include "unxs.h"
#include "nifty.h"

#define NANPX		NAND64


int
main(void)
{
	clob_t c;
	clob_oid_t o1, o2 = {};

	c = make_clob();
	for (size_t j = 0U; j < 10000000U; j++) {
		o1 = clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_LONG, {2.dd, 0.dd}, 100.dd});
		clob_del(c, o2);
		o2 = clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_LONG, {2.dd, 0.dd}, 102.dd});
		clob_del(c, o1);
	}

	clob_prnt(c);

	free_clob(c);
	return 0;
}
