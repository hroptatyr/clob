#include <stdio.h>
#include "dfp754_d64.h"
#include "clob.h"
#include "unxs.h"


int
main(void)
{
	clob_t c;
	clob_oid_t i;

	c = make_clob();

	i = clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, {50.0dd, 0.0dd}, .lmt = 1.23228dd});
	clob_prnt(c);

	unxs_order(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_SHORT, {25.dd, 0.0dd}}, NANPX);

	clob_prnt(c);

	printf("%d\n", clob_del(c, i));

	clob_prnt(c);

	free_clob(c);
	return 0;
}
