/*** plqu_val.h -- values of price level queues ***/
#if !defined INCLUDED_plqu_val_h_
#define INCLUDED_plqu_val_h_
#include <stdbool.h>
#include "clob_val.h"

/* order bits essential for execution */
typedef struct {
	/* quantity */
	qty_t qty;
	/* metronome stamp, strictly increasing */
	metr_t tim;
} plqu_val_t;

#define plqu_val_nil	((plqu_val_t){.tim = 0U})

static inline bool
plqu_val_nil_p(plqu_val_t v)
{
	return v.tim == 0U;
}


/* not used by plqu but nice to define here */
static inline __attribute__((pure, const)) plqu_val_t
plqu_val_exe(plqu_val_t v1, plqu_val_t v2)
{
/* V1 is executed against V2, use up hidden liquidity of V1 first */
	return (plqu_val_t){qty_exe(v1.qty, v2.qty), v1.tim};
}

#endif	/* INCLUDED_plqu_h_ */
