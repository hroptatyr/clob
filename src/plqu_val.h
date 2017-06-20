/*** plqu_val.h -- values of price level queues ***/
#if !defined INCLUDED_plqu_val_h_
#define INCLUDED_plqu_val_h_
#include <stdbool.h>
#include "clob_val.h"

/* quantities with hidden liquidity*/
typedef struct {
	/* visible liquidity */
	qx_t vis;
	/* hidden liquidity */
	qx_t hid;
	/* metronome stamp, strictly increasing */
	metr_t tim;
} plqu_val_t;

#define plqu_val_nil	((plqu_val_t){0.dd})

static inline bool
plqu_val_nil_p(plqu_val_t v)
{
	return v.vis + v.hid <= 0.dd;
}


/* not used by plqu but nice to define here */
#include "nifty.h"

static inline __attribute__((pure, const)) qx_t
plqu_val_tot(plqu_val_t v)
{
	return v.vis + v.hid;
}

static inline __attribute__((pure, const)) plqu_val_t
plqu_val_add(plqu_val_t v1, plqu_val_t v2)
{
	return (plqu_val_t){v1.vis + v2.vis, v1.hid + v2.hid, min(v1.tim, v2.tim)};
}

static inline __attribute__((pure, const)) plqu_val_t
plqu_val_sub(plqu_val_t v1, plqu_val_t v2)
{
	return (plqu_val_t){v1.vis - v2.vis, v1.hid - v2.hid, min(v1.tim, v2.tim)};
}

static inline __attribute__((pure, const)) plqu_val_t
plqu_val_exe(plqu_val_t v1, plqu_val_t v2)
{
/* V1 is executed against V2, use up hidden liquidity of V1 first */
	qx_t nuh = min(v1.hid, v2.vis + v2.hid);
	qx_t nuv = max(0.dd, v2.vis + v2.hid - v1.hid);
	return (plqu_val_t){v1.vis - nuv, v1.hid - nuh, min(v1.tim, v2.tim)};
}

static inline __attribute__((pure, const)) plqu_val_t
plqu_val_min(plqu_val_t v1, plqu_val_t v2)
{
	return (plqu_val_t){min(v1.vis, v2.vis), min(v1.hid, v2.hid), min(v1.tim, v2.tim)};
}

static inline __attribute__((pure, const)) plqu_val_t
plqu_val_max(plqu_val_t v1, plqu_val_t v2)
{
	return (plqu_val_t){max(v1.vis, v2.vis), max(v1.hid, v2.hid), max(v1.tim, v2.tim)};
}

#endif	/* INCLUDED_plqu_h_ */
