#include <stdio.h>
#include "dfp754_d64.h"
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

	/* xetra example 1 */
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, {100.dd, 0.0dd}, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, {100.dd, 0.0dd}, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_BID, {100.dd, 0.0dd}});

	clob_prnt(c);

	/* eat into the limits */
	unxs_order(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_SHORT, {200.dd, 0.0dd}}, NANPX);

#if 0
	for (size_t i = 0U; i < n; i++) {
		printf("%f @ %f  %u %u %f %zu  v  %u %u %f %zu\n",
		       (double)x[i].x.qty, (double)x[i].x.prc,
		       x[i].o[CLOB_SIDE_MAKER].typ, x[i].o[CLOB_SIDE_MAKER].sid, (double)x[i].o[CLOB_SIDE_MAKER].prc, x[i].o[CLOB_SIDE_MAKER].qid,
		       x[i].o[CLOB_SIDE_TAKER].typ, x[i].o[CLOB_SIDE_TAKER].sid, (double)x[i].o[CLOB_SIDE_TAKER].prc, x[i].o[CLOB_SIDE_TAKER].qid);
	}
#endif

	clob_prnt(c);

	rc = c.exe->n != 2U ||
		c.exe->x[0U].qty != 100.dd ||
		c.exe->x[0U].prc != 198.0dd ||
		c.exe->x[1U].qty != 100.dd ||
		c.exe->x[1U].prc != 198.0dd ||
		0;

	free_unxs(c.exe);
	free_clob(c);
	return rc;
}
