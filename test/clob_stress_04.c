#include <stdio.h>
#include "clob.h"
#include "btree.h"
#include "btree_val.h"
#include "unxs.h"
#include "mmod-auction.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;
	qx_t tot[2U] = {0.dd, 0.dd};

	c = make_clob();
	for (size_t j = 0U; j < 1000U; j++) {
		clob_side_t s = rand() % 2;
		qx_t q = 2.dd + rand() % 4;
		tot[s] += q;
		clob_add(c, (clob_ord_t){TYPE_LMT, s, q, 0.dd, .lmt = 100.00dd + rand() % 81});
	}

	clob_prnt(c);
	{
		unxs_exsc_t x[100000U];
		mmod_auc_t auc = mmod_auction(c);
		size_t n = unxs_mass_sc(x, countof(x), c, auc.prc, auc.qty);
		btree_key_t a, b;

		printf("AUC %f %f %f\n", (double)auc.prc, (double)auc.qty, (double)auc.imb);

		printf("TOT SUB %f %f\n", (double)tot[0], (double)tot[1]);
		btree_top(c.lmt[SIDE_ASK], &a);
		btree_top(c.lmt[SIDE_BID], &b);

		qx_t q = 0.dd;
		for (size_t i = 0U; i < n; i++) {
			q += x[i].x.qty;
		}
		printf("TRA\t%f\t%f\t%zu\n", (double)x->x.prc, (double)q, n);
		printf("QUO\t%f\t%f\n", (double)b, (double)a);
	}
	clob_prnt(c);

	free_clob(c);
	return 0;
}
