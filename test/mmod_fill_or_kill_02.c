#include <stdio.h>
#include "clob.h"
#include "mmod-fok.h"
#include "dfp754_d64.h"
#include "nifty.h"

#define NANPX	NAND64


int
main(void)
{
	clob_t c;

	c = make_clob();

	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, 100.dd, 0.0dd, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, 300.dd, 0.0dd, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_BID, 100.dd, 0.0dd, .lmt = 196.0dd});

	clob_prnt(c);

	int f1 = mmod_fok(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_LONG, {200.dd}, .lmt = 200.dd});
	int f2 = mmod_fok(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_LONG, {200.dd}, .lmt = 197.dd});
	int f3 = mmod_fok(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_SHORT, {100.dd}, .lmt = 195.dd});
	int f4 = mmod_fok(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_SHORT, {200.dd}, NANPX});

	printf("%d %d %d %d\n", f1, f2, f3, f4);

	clob_prnt(c);

	free_clob(c);
	return !f1 || f2 || !f3 || f4;
}
