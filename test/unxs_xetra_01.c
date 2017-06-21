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

	/* xetra example 1 */
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 100.dd, 0.0dd, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 300.dd, 0.0dd, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 400.dd, 0.0dd, .lmt = 197.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 300.dd, 0.0dd, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 100.dd, 0.0dd, .lmt = 196.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 300.dd, 0.0dd, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 200.dd, 0.0dd, .lmt = 199.0dd});

	clob_prnt(c);

	{
		unxs_exe_t x[16U];
		mmod_auc_t a;
		size_t n;

		puts("UNX AUCTION");
		a = mmod_auction(c);
		n = unxs_mass(x, countof(x), c, a.prc, a.qty);

		for (size_t i = 0U; i < n; i++) {
			printf("%f @ %f\n", (double)x[i].qty, (double)x[i].prc);
		}
	}

	clob_prnt(c);

	free_clob(c);
	return 0;
}
