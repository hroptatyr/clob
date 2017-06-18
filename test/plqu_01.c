#include "config.h"
#include <stdio.h>
#include "plqu.h"


int
main(void)
{
	plqu_t q;

	q = make_plqu();

	printf("%zu\n", plqu_add(q, (plqu_val_t){0.1dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.2dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.3dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.4dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.5dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.6dd}));
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.7dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.8dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.9dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.01dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.11dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.21dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.31dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.41dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.51dd}));
	printf("%zu\n", plqu_add(q, (plqu_val_t){0.61dd}));
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);
	printf("%f\n", (double)plqu_top(q).vis);
	printf("%f\n", (double)plqu_pop(q).vis);

	free_plqu(q);
	return 0;
}
