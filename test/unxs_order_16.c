#include <stdio.h>
#include "clob.h"
#include "unxs.h"
#include "nifty.h"


int
main(void)
{
	unxs_exbi_t x[16U];
	size_t n;
	clob_t c;

	c = make_clob();

	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, {100.dd, 0.0dd}, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, {10.dd, 50.0dd}, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, {10.dd, 50.0dd}, .lmt = 197.0dd});

	clob_prnt(c);

	/* eat into the limits */
	n = unxs_order(x, countof(x), c, (clob_ord_t){TYPE_LMT, SIDE_SHORT, {80.dd, 0.0dd}, .lmt = 197.dd}, NANPX);

	for (size_t i = 0U; i < n; i++) {
		printf("%f @ %f  %u %u %f %zu  v  %u %u %f %zu\n",
		       (double)x[i].x.qty, (double)x[i].x.prc,
		       x[i].o[SIDE_MAKER].typ, x[i].o[SIDE_MAKER].sid, (double)x[i].o[SIDE_MAKER].prc, x[i].o[SIDE_MAKER].qid,
		       x[i].o[SIDE_TAKER].typ, x[i].o[SIDE_TAKER].sid, (double)x[i].o[SIDE_TAKER].prc, x[i].o[SIDE_TAKER].qid);
	}

	clob_prnt(c);

	free_clob(c);
	return n != 3U ||
		x[0U].x.qty != 0.dd ||
		x[1U].x.qty != 60.dd || x[1U].x.prc != 198.0dd ||
		x[2U].x.qty != 20.dd || x[2U].x.prc != 197.0dd ||
		0;
}
