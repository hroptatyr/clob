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

	/* xetra example 1 */
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, {100.dd, 0.0dd}, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, {100.dd, 0.0dd}, .lmt = 198.0dd});

	clob_prnt(c);

	unxs_order(c, (clob_ord_t){TYPE_MKT, SIDE_SHORT, {5.dd, 0.0dd}}, NANPX);

	for (size_t i = 0U; i < c.exe->n; i++) {
		printf("%f @ %f  %u %u %f %zu  v  %u %u %f %zu\n",
		       (double)c.exe->x[i].qty, (double)c.exe->x[i].prc,
		       c.exe->o[i * MODE_BI + SIDE_MAKER].typ,
		       c.exe->o[i * MODE_BI + SIDE_MAKER].sid,
		       (double)c.exe->o[i * MODE_BI + SIDE_MAKER].prc,
		       c.exe->o[i * MODE_BI + SIDE_MAKER].qid,

		       c.exe->o[i * MODE_BI + SIDE_TAKER].typ,
		       c.exe->o[i * MODE_BI + SIDE_TAKER].sid,
		       (double)c.exe->o[i * MODE_BI + SIDE_TAKER].prc,
		       c.exe->o[i * MODE_BI + SIDE_TAKER].qid);
	}

	clob_prnt(c);

	rc = c.exe->n != 1U || c.exe->x->qty != 5.dd || c.exe->x->prc != 198.0dd;

	free_unxs(c.exe);
	free_clob(c);
	return rc;
}
