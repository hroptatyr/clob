#include <stdio.h>
#include "clob.h"


int
main(void)
{
	clob_t c;
	clob_oid_t i;

	c = make_clob();

	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, .lmt = 1.23228dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);	                         
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, .lmt = 1.23230dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);                          
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_BID, .lmt = 1.23228dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);	                         
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, .lmt = 1.23229dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);	                         
	i = clob_add(c, (clob_ord_t){TYPE_LMT, SIDE_ASK, .lmt = 1.23230dd, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);

	i = clob_add(c, (clob_ord_t){TYPE_MID, SIDE_ASK, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_MID, SIDE_ASK, 0.1dd, 0.0dd});
	printf("%zu\n", i.qid);
	i = clob_add(c, (clob_ord_t){TYPE_MID, SIDE_ASK, 0.5dd, 0.0dd});
	printf("%zu\n", i.qid);

	printf("%d\n", clob_del(c, i));
	printf("%d\n", clob_del(c, i));
	i.qid++;
	printf("%d\n", clob_del(c, i));

	clob_prnt(c);

	free_clob(c);
	return 0;
}
