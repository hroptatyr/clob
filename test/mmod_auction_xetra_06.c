#include <stdio.h>
#include "clob.h"
#include "unxs.h"
#include "mmod-auction.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;

	c = make_clob();

	/* xetra example 6 */
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 0.dd, 0.0dd, .lmt = 202.0dd});
	clob_add(c, (clob_ord_t){TYPE_MKT, SIDE_SHORT, 100.dd, 0.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 0.dd, 0.0dd, .lmt = 199.0dd});
	clob_add(c, (clob_ord_t){TYPE_MKT, SIDE_LONG, 200.dd, 0.0dd});

	clob_prnt(c);

	mmod_auc_t auc = mmod_auction(c);
	printf("AUCTION %f @ %f ~%f\n", (double)auc.qty, (double)auc.prc, (double)auc.imb);

	free_clob(c);
	return auc.prc != 202.dd || auc.qty != 100.dd || auc.imb != 100.dd;
}
