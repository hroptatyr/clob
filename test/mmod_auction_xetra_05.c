#if defined HAVE_CONFIG_H
# include "config.h"
#endif	/* HAVE_CONFIG_H */
#include <stdio.h>
#if defined HAVE_DFP754_H
# include <dfp754.h>
#elif defined HAVE_DFP_STDLIB_H
# include <dfp/stdlib.h>
#elif defined HAVE_DECIMAL_H
# include <decimal.h>
#endif
#include "dfp754_d64.h"
#include "clob.h"
#include "unxs.h"
#include "mmod-auction.h"
#include "nifty.h"

#define isnanpx		isnand64


int
main(void)
{
	clob_t c;

	c = make_clob();

	/* xetra example 5 */
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, 300.dd, 0.0dd, .lmt = 202.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, 200.dd, 0.0dd, .lmt = 201.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, 100.dd, 0.0dd, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, 400.dd, 0.0dd, .lmt = 199.0dd});

	clob_prnt(c);

	mmod_auc_t auc = mmod_auction(c);
	printf("AUCTION %f @ %f ~%f\n", (double)auc.qty, (double)auc.prc, (double)auc.imb);

	free_clob(c);
	return !isnanpx(auc.prc) || auc.qty != 0.dd || auc.imb != 0.dd;
}
