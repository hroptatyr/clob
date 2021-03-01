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
#include <stdio.h>
#include "dfp754_d64.h"
#include "clob.h"
#include "unxs.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;
	int rc;

	c = make_clob();
	c.exe = make_unxs(MODE_BI);

	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_ASK, {100.dd, 0.0dd}, .lmt = 200.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_BID, {100.dd, 0.0dd}, .lmt = 198.0dd});
	clob_add(c, (clob_ord_t){CLOB_TYPE_MKT, CLOB_SIDE_BID, {100.dd, 0.0dd}});

	clob_prnt(c);

	/* less book than order */
	unxs_order(c, (clob_ord_t){CLOB_TYPE_LMT, CLOB_SIDE_SHORT, {500.dd, 0.0dd}, .lmt = 197.dd}, NANPX);

	clob_prnt(c);

	rc = c.exe->n != 2U ||
		c.exe->x[0U].qty != 100.dd ||
		c.exe->x[0U].prc != 198.0dd ||
		c.exe->x[1U].qty != 100.dd ||
		c.exe->x[1U].prc != 198.0dd ||
		0;

	free_unxs(c.exe);
	free_clob(c);
	return rc;
}
