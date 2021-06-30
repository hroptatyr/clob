#include <stdio.h>
#include "btree.h"


int
main(void)
{
	btree_t t;
	int rc = 0;

	t = make_btree(true);

	for (size_t j = 0U; j < 100U; j++) {
		btree_val_t *v = btree_put(t, 1.dd+j);
		if (btree_val_nil_p(*v)) {
			*v = (btree_val_t){make_plqu(), {1.dd+j}};
			printf("%p ", v);
		} else {
			rc = 1;
			break;
		}

		v = btree_get(t, 1.dd+j);
		if (v && !btree_val_nil_p(*v)) {
			printf(" %p %f\n", v, (double)v->sum.dis);
		} else {
			puts("");
			rc = 1;
			break;
		}
	}

	free_btree(t);
	return rc;
}
