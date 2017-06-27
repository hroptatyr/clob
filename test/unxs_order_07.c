#include <stdio.h>
#include "clob.h"
#include "unxs.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;

	c = make_clob();

	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, {100.dd, 0.0dd}, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, {10.dd, 50.0dd}, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, {10.dd, 50.0dd}, .lmt = 197.0dd});

	clob_prnt(c);

	/* eat into the limits */
	unxs_order(c, (clob_ord_t){TYPE_MKT, SIDE_SHORT, {80.dd, 0.0dd}}, NANPX);

	clob_prnt(c);

	free_clob(c);
	return 0;
}
