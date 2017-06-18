/*** plqu_val.h -- values of price level queues ***/
#if !defined INCLUDED_plqu_val_h_
#define INCLUDED_plqu_val_h_

/* quantities with hidden liquidity*/
typedef struct {
	_Decimal64 vis;
	_Decimal64 hid;
} plqu_val_t;

#define plqu_val_0	((plqu_val_t){0.dd, 0.dd})

static inline __attribute__((pure, const)) plqu_val_t
plqu_val_add(plqu_val_t x, plqu_val_t y)
{
	return (plqu_val_t){x.vis + y.vis, x.hid + y.hid};
}

static inline __attribute__((pure, const)) plqu_val_t
plqu_val_sub(plqu_val_t x, plqu_val_t y)
{
	return (plqu_val_t){x.vis - y.vis, x.hid - y.hid};
}

#endif	/* INCLUDED_plqu_h_ */
