#include <stdio.h>
#include "clob.h"
#include "mmod-ioc.h"
#include "dfp754_d64.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;

	c = make_clob();

	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, {100.dd, 0.0dd}, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, {300.dd, 0.0dd}, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_BID, {300.dd, 0.0dd}});

	clob_prnt(c);

	int f1 = mmod_ioc(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_LONG, {200.dd}, NANPX});
	int f2 = mmod_ioc(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_LONG, {500.dd}, .lmt = 197.0dd});
	int f3 = mmod_ioc(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_LONG, {400.dd}, 199.0dd});
	int f4 = mmod_ioc(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_SHORT, {100.dd}, 195.dd});
	int f5 = mmod_ioc(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_SHORT, {100.dd}, NANPX});

	printf("%d %d %d %d %d\n", f1, f2, f3, f4, f5);

	clob_prnt(c);

	free_clob(c);
	return !f1 || f2 || !f3 || !f4 || f5;
}
