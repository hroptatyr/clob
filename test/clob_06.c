#include <stdio.h>
#include "dfp754_d64.h"
#include "clob.h"
#include "nifty.h"

#define NANPX		NAND64


int
main(void)
{
	clob_t c;
	clob_aggiter_t i;
	size_t j = 0U;

	c = make_clob();

	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, {100.dd, 0.0dd}, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, {100.dd, 0.0dd}, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, {10.dd, 50.0dd}, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, {10.dd, 50.0dd}, .lmt = 197.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, {10.dd, 50.0dd}, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_SHORT, {80.dd, 0.0dd}});

	puts("LMT+BID");
	i = clob_aggiter(c, CLOB_TYPE_LMT, CLOB_SIDE_BID);
	for (; clob_aggiter_next(&i); j++) {
		printf("%f %f\n", (double)i.p, (double)(i.q.dis + i.q.hid));
	}
	puts("LMT+ASK");
	i = clob_aggiter(c, CLOB_TYPE_LMT, CLOB_SIDE_ASK);
	for (; clob_aggiter_next(&i); j++) {
		printf("%f %f\n", (double)i.p, (double)(i.q.dis + i.q.hid));
	}
	puts("MKT+BID");
	i = clob_aggiter(c, CLOB_TYPE_MKT, CLOB_SIDE_BID);
	for (; clob_aggiter_next(&i); j++) {
		printf("%f %f\n", (double)i.p, (double)(i.q.dis + i.q.hid));
	}
	puts("MKT+ASK");
	i = clob_aggiter(c, CLOB_TYPE_MKT, CLOB_SIDE_ASK);
	for (; clob_aggiter_next(&i); j++) {
		printf("%f %f\n", (double)i.p, (double)(i.q.dis + i.q.hid));
	}

	free_clob(c);
	return j != 4;
}
