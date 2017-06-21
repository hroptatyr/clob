#include <stdio.h>
#include "clob.h"
#include "unxs.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;

	c = make_clob();

	/* xetra example 1 */
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 100.dd, 0.0dd, .lmt = 203.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 300.dd, 0.0dd, .lmt = 199.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 200.dd, 0.0dd, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 100.dd, 0.0dd, .lmt = 197.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 200.dd, 0.0dd, .lmt = 201.0dd});
	clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 300.dd, 0.0dd, .lmt = 202.0dd});

	clob_prnt(c);

	{
		unxs_exe_t x[16U];
		puts("UNX AUCTION");
		unxs_auction(x, countof(x), c);
	}

	clob_prnt(c);

	free_clob(c);
	return 0;
}