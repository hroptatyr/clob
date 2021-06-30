#include <stdio.h>
#include "dfp754_d64.h"
#include "clob.h"
#include "btree.h"
#include "btree_val.h"
#include "unxs.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;
	clob_ord_t o1;
	clob_ord_t o2;

	c = make_clob();
	for (size_t j = 1U; j <= 100U; j++) {
		//o1 = unxs_order(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, {0.dd+j, 0.dd}, 6000.dd+j}, NANPX);
		//if (o1.qty.dis + o1.qty.hid > 0.dd) {
		o1 = (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, {0.dd+j, 0.dd}, 6000.dd+j};
		printf("%zu ", j);clob_add(c, o1);
		//clob_prnt(c);
		//}
	}

	clob_prnt(c);

	puts("COUNTER");

	o2 = unxs_order(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, {5050.dd, 0.dd}, 6100.dd}, NANPX);
	if (o2.qty.dis + o2.qty.hid > 0.dd) {
		clob_add(c, o2);
	}

	clob_prnt(c);

	free_clob(c);
	return 0;
}
