#include <stdio.h>
#include "clob.h"
#include "unxs.h"
#include "nifty.h"


int
main(void)
{
	clob_t c;

	c = make_clob();

	for (size_t i = 0U; i < 10000000U; i++) {
		clob_add(c, (clob_ord_t){CLOB_TYPE_LMT, rand() % 2, 2 + rand() % 4, 0.dd, .lmt = 100 + rand() % 100});
	}

	clob_prnt(c);
	free_clob(c);
	return 0;
}
