#include <stdio.h>
#include "clob.h"
#include "unxs.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;
	clob_oid_t i;

	c = make_clob();

	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 0.5dd, 0.0dd, .lmt = 1.23227dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 0.5dd, 0.0dd, .lmt = 1.23230dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 0.5dd, 0.0dd, .lmt = 1.23227dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 0.5dd, 0.0dd, .lmt = 1.23228dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 0.4dd, 0.0dd, .lmt = 1.23230dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 0.5dd, 0.0dd, .lmt = 1.23231dd});
	printf("%zu\n", i.qid);

	i = clob_add(c, (clob_ord_t){TYPE_MID, SIDE_ASK, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_MID, SIDE_BID, 0.1dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_MID, SIDE_ASK, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);

	printf("%d\n", clob_del(c, i));
	printf("%d\n", clob_del(c, i));
	i.qid++;
	printf("%d\n", clob_del(c, i));

	clob_prnt(c);

	/* add marketable limit order */
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 0.7dd, 0.0dd, .lmt = 1.23226dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 0.4dd, 0.0dd, .lmt = 1.23229dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 0.2dd, 0.0dd, .lmt = 1.23231dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 0.8dd, 0.0dd, .lmt = 1.23226dd});
	printf("%zu\n", i.qid);

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
