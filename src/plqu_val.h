/*** plqu_val.h -- values of price level queues ***/
#if !defined INCLUDED_plqu_val_h_
#define INCLUDED_plqu_val_h_
#include <stdbool.h>
#include <stdint.h>
#include "clob_val.h"

/* order bits essential for execution */
typedef struct {
	/** quantity */
	qty_t qty;
	/** metronome stamp, strictly increasing */
	metr_t tim;
	/** user data */
	uintptr_t usr;
} plqu_val_t;

#define plqu_val_nil	((plqu_val_t){.tim = 0U})

static inline bool
plqu_val_nil_p(plqu_val_t v)
{
	return v.tim == 0U;
}

#endif	/* INCLUDED_plqu_h_ */
