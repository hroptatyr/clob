#include <stdio.h>
#include "clob.h"
#include "unxs.h"
#include "mmod-auction.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;

	c = make_clob();

	for (size_t i = 0U; i < 10000000U; i++) {
		clob_add(c, (clob_ord_t){TYPE_LMT, rand() % 2, 2 + rand() % 4, 0.dd, .lmt = 100 + rand() % 100});
		if (!((i - 1U) % 100U)) {
			unxs_exe_t x[100000U];
			mmod_auc_t a = mmod_auction(c);
			size_t n = unxs_mass_bi(x, countof(x), c, a.prc, a.qty);
			//clob_prnt(c);
		}
	}

	clob_prnt(c);
	free_clob(c);
	return 0;
}
