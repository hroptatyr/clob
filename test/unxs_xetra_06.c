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
	clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_LONG, 200.dd, 0.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_SHORT, 100.dd, 0.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, 0.dd, 0.0dd, .lmt = 202.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, 0.dd, 0.0dd, .lmt = 199.0dd});

	clob_prnt(c);

	{
		mmod_auc_t a;

		puts("UNX AUCTION");
		a = mmod_auction(c);
		unxs_auction(c, a.prc, a.qty);
	}

	clob_prnt(c);

	free_clob(c);
	return 0;
}
