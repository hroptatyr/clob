#include <stdio.h>
#include "clob.h"
#include "unxs.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;
	int rc;

	c = make_clob();
	c.exe = make_unxs(MODE_BI);

	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, {100.dd, 0.0dd}, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, {10.dd, 50.0dd}, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, {10.dd, 50.0dd}, .lmt = 197.0dd});

	clob_prnt(c);

	/* eat into the limits */
	unxs_order(c, (clob_ord_t){TYPE_LMT, SIDE_SHORT, {80.dd, 0.0dd}, .lmt = 197.dd}, NANPX);

	clob_prnt(c);

	rc = c.exe->n != 2U ||
		c.exe->x[0U].qty != 60.dd ||
		c.exe->x[0U].prc != 198.0dd ||
		c.exe->x[1U].qty != 20.dd ||
		c.exe->x[1U].prc != 197.0dd ||
		0;

	free_unxs(c.exe);
	free_clob(c);
	return rc;
}
