#include <stdio.h>
#include "clob.h"
#include "unxs.h"


int
main(void)
{
	clob_t c;
	clob_oid_t i;

	c = make_clob();

	i = clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, 0.5dd, 0.0dd, .lmt = 1.23228dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, 0.5dd, 0.0dd, .lmt = 1.23230dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, 0.5dd, 0.0dd, .lmt = 1.23228dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, 0.5dd, 0.0dd, .lmt = 1.23229dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, 0.5dd, 0.0dd, .lmt = 1.23230dd});
	printf("%zu\n", i.qid);

	i = clob_add(c, (clob_ord_t){CLOB_TYPE_MID, CLOB_SIDE_ASK, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_MID, CLOB_SIDE_BID, 0.1dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_MID, CLOB_SIDE_ASK, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);

	printf("%d\n", clob_del(c, i));
	printf("%d\n", clob_del(c, i));
	i.qid++;
	printf("%d\n", clob_del(c, i));

	clob_prnt(c);

	/* add marketable limit order */
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, 0.7dd, 0.0dd, .lmt = 1.23227dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, 0.3dd, 0.0dd, .lmt = 1.23229dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, 0.8dd, 0.0dd, .lmt = 1.23226dd});
	printf("%zu\n", i.qid);

	/* some market orders */
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_ASK, 0.2dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_ASK, 0.2dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_ASK, 0.2dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_ASK, 0.2dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_BID, 0.3dd, 0.0dd});
	printf("%zu\n", i.qid);

#if 0
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_ASK, 0.2dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_ASK, 0.2dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_ASK, 0.2dd, 0.0dd});
	printf("%zu\n", i.qid);
#endif

	clob_prnt(c);

	puts("UNX MKT/LMT");
	//clob_unx_mkt_lmt(c);
	puts("UNX LMT/LMT");
	//clob_unx_lmt_lmt(c);
	puts("UNX MID/MID");
	//clob_unx_mid_mid(c);

	clob_prnt(c);

	free_clob(c);
	return 0;
}
