#if defined HAVE_CONFIG_H
# include "config.h"
#endif	/* HAVE_CONFIG_H */
#include <stdio.h>
#if defined HAVE_DFP754_H
# include <dfp754.h>
#elif defined HAVE_DFP_STDLIB_H
# include <dfp/stdlib.h>
#elif defined HAVE_DECIMAL_H
# include <decimal.h>
#endif
#include <stdio.h>
#include "clob.h"
#include "unxs.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;
	int rc;
	clob_oid_t o;

	c = make_clob();
	c.exe = make_unxs(MODE_BI);

	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, {100.dd, 0.0dd}, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, {100.dd, 0.0dd}, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){TYPE_MKT, SIDE_BID, {100.dd, 0.0dd}});

	clob_prnt(c);

	/* less book than order */
	o = unxs_order(c, (clob_ord_t){TYPE_MKT, SIDE_SHORT, {500.dd, 0.0dd}}, NANPX);

#if 0
	for (size_t i = 0U; i < n; i++) {
		printf("%f @ %f  %u %u %f %zu  v  %u %u %f %zu\n",
		       (double)x[i].x.qty, (double)x[i].x.prc,
		       x[i].o[SIDE_MAKER].typ, x[i].o[SIDE_MAKER].sid, (double)x[i].o[SIDE_MAKER].prc, x[i].o[SIDE_MAKER].qid,
		       x[i].o[SIDE_TAKER].typ, x[i].o[SIDE_TAKER].sid, (double)x[i].o[SIDE_TAKER].prc, x[i].o[SIDE_TAKER].qid);
	}
#endif

	clob_prnt(c);

	rc = c.exe->n != 2U ||
		o.qid == 0U ||
		c.exe->x[0U].qty != 100.dd ||
		c.exe->x[0U].prc != 198.0dd ||
		c.exe->x[1U].qty != 100.dd ||
		c.exe->x[1U].prc != 198.0dd ||
		0;

	free_unxs(c.exe);
	free_clob(c);
	return rc;
}
