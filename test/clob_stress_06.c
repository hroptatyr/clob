#include <stdio.h>
#include "clob.h"
#include "btree.h"
#include "btree_val.h"
#include "unxs.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;
	unxs_exbi_t x[16U];

	c = make_clob();
	clob_add(c, (clob_ord_t){TYPE_MKT, SIDE_LONG, {2.dd, 0.dd}});
	clob_add(c, (clob_ord_t){TYPE_MKT, SIDE_SHORT, {2.dd, 1.dd}});
	for (size_t j = 0U; j < 1000000U; j++) {
		clob_side_t s = rand() % 2;
		qx_t hid = rand() % 4;
		px_t lmt = 100.00dd + rand() % 81;

		clob_add(c, (clob_ord_t){TYPE_LMT, s, {2.dd, hid}, .lmt = lmt});
		unxs_order(x, countof(x), c, (clob_ord_t){TYPE_MKT, s ^ 1, {2.dd, (rand() % 3) + 0.00}}, NANPX);
	}

	clob_prnt(c);

	free_clob(c);
	return 0;
}
