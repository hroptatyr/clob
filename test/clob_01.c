#include <stdio.h>
#include "clob.h"


int
main(void)
{
	clob_t c;
	clob_oid_t i;

	c = make_clob();

	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 1.23228dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);	                         
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 1.23230dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);                          
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, 1.23228dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);	                         
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 1.23229dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);	                         
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, 1.23230dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);

	i = clob_add(c, (clob_ord_t){TYPE_MID, SIDE_ASK, 0.dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_MID, SIDE_ASK, 0.dd, 0.1dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_MID, SIDE_ASK, 0.dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);

	printf("%d\n", clob_del(c, i));
	printf("%d\n", clob_del(c, i));
	i.qid++;
	printf("%d\n", clob_del(c, i));

	clob_quo_t b = clob_top(c, TYPE_LMT, SIDE_BID);
	clob_quo_t a = clob_top(c, TYPE_LMT, SIDE_ASK);

	printf("%f %f %f %f\n", (double)b.q, (double)b.p, (double)a.p, (double)a.q);

	free_clob(c);
	return 0;
}
