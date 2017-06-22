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
	clob_add(c, (clob_ord_t){TYPE_MKT, SIDE_LONG, {2.dd, 0.dd}});
	clob_add(c, (clob_ord_t){TYPE_MKT, SIDE_SHORT, {2.dd, 1.dd}});
	for (size_t j = 0U; j < 998U; j++) {
		clob_side_t s = rand() % 2;
		qx_t q = 2.dd + rand() % 4;
		tot[s] += q;
		clob_add(c, (clob_ord_t){TYPE_LMT, s, {q, 0.dd}, .lmt = 100.00dd + rand() % 81});
	}

	clob_prnt(c);
	{
		unxs_exbi_t x[100000U];
		mmod_auc_t auc = mmod_auction(c);
		size_t n = unxs_mass_bi(x, countof(x), c, auc.prc, auc.qty);
		btree_key_t a, b;

		printf("AUC %f %f %f\n", (double)auc.prc, (double)auc.qty, (double)auc.imb);

		printf("TOT SUB %f %f\n", (double)tot[0], (double)tot[1]);
		btree_top(c.lmt[SIDE_ASK], &a);
		btree_top(c.lmt[SIDE_BID], &b);

		qx_t q = 0.dd;
		for (size_t i = 0U; i < n; i++) {
			//q += x[i].x.qty;
			printf("%f @ %f  %u %u %f %zu  v  %u %u %f %zu\n",
			       (double)x[i].x.qty, (double)x[i].x.prc,
			       x[i].o[SIDE_BUYER].typ, x[i].o[SIDE_BUYER].sid, (double)x[i].o[SIDE_BUYER].prc, x[i].o[SIDE_BUYER].qid,
			       x[i].o[SIDE_SELLER].typ, x[i].o[SIDE_SELLER].sid, (double)x[i].o[SIDE_SELLER].prc, x[i].o[SIDE_SELLER].qid);
		}
		printf("TRA\t%f\t%f\t%zu\n", (double)x->x.prc, (double)q, n);
		printf("QUO\t%f\t%f\n", (double)b, (double)a);
	}
	clob_prnt(c);

	free_clob(c);
	return 0;
}
