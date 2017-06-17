/*** plqu_val.h -- values of price level queues ***/
#if !defined INCLUDED_plqu_val_h_
#define INCLUDED_plqu_val_h_

typedef _Decimal64 plqu_val_t;

#define plqu_val_0	0.dd

static inline __attribute__((pure, const)) plqu_val_t
plqu_val_add(plqu_val_t x, plqu_val_t y)
{
	return x + y;
}

static inline __attribute__((pure, const)) plqu_val_t
plqu_val_sub(plqu_val_t x, plqu_val_t y)
{
	return x - y;
}

#endif	/* INCLUDED_plqu_h_ */
