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

	c = make_clob();
	clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_LONG, {2.dd, 0.dd}});
	clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_SHORT, {2.dd, 1.dd}});
	for (size_t j = 0U; j < 1000000U; j++) {
		clob_side_t s = rand() % 2;
		qx_t hid = rand() % 4;
		px_t lmt = 100.00dd + rand() % 81;

		clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, s, {2.dd, hid}, .lmt = lmt});
		unxs_order(c, (clob_ord_t){CLOB_TYPE_MKT, s ^ 1, {2.dd, (rand() % 3) + 0.00}}, NANPX);
	}

	clob_lvl2(c);

	free_clob(c);
	return 0;
}
