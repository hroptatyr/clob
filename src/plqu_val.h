/*** plqu_val.h -- values of price level queues ***/
#pragma once
#include <stdbool.h>

/* metronome type */
typedef size_t metr_t;

/* quantities with hidden liquidity*/
typedef struct {
	_Decimal64 vis;
	_Decimal64 hid;
	metr_t tim;
} plqu_val_t;

#define plqu_val_nil	((plqu_val_t){0.dd})

static inline bool
plqu_val_nil_p(plqu_val_t v)
{
	return v.vis <= 0.dd;
}


/* not used by plqu but nice to define here */
static inline __attribute__((pure, const)) plqu_val_t
plqu_val_add(plqu_val_t v1, plqu_val_t v2)
{
	return (plqu_val_t){v1.vis + v2.vis, v1.hid + v2.hid};
}

static inline __attribute__((pure, const)) plqu_val_t
plqu_val_sub(plqu_val_t v1, plqu_val_t v2)
{
	return (plqu_val_t){v1.vis - v2.vis, v1.hid - v2.hid};
}
