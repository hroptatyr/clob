#include <stdio.h>
#include "clob.h"
#include "mmod-pdo.h"
#include "dfp754_d64.h"
#include "nifty.h"

#define NANPX	NAND64


int
main(void)
{
	clob_t c;

	c = make_clob();

	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, 300.dd, 0.0dd, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, 100.dd, 0.0dd, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, 100.dd, 0.0dd, .lmt = 196.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, 300.dd, 0.0dd, .lmt = 197.0dd});

	clob_prnt(c);

	mmod_pdo_t f1 = mmod_pdo(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_LONG, {200.dd}, NANPX});
	mmod_pdo_t f2 = mmod_pdo(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_LONG, {500.dd}, NANPX});
	mmod_pdo_t f3 = mmod_pdo(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_SHORT, {100.dd}, NANPX});
	mmod_pdo_t f4 = mmod_pdo(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_SHORT, {400.dd}, 0.dd});

	printf("%f %f  %f %f  %f %f  %f %f\n",
	       (double)f1.base, (double)f1.term,
	       (double)f2.base, (double)f2.term,
	       (double)f3.base, (double)f3.term,
	       (double)f4.base, (double)f4.term);

	clob_prnt(c);

	free_clob(c);
	return f1.base != 200.dd || f1.term != 39800.dd ||
		f2.base != 400.dd || f2.term != 79800.dd ||
		f3.base != 100.dd || f3.term != 19700.dd ||
		f4.base != 300.dd || f4.term != 59100.dd;
}
