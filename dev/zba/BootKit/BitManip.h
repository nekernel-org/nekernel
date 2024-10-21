/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#ifndef __BITMANIP_H__
#define __BITMANIP_H__

/// Name: Bits API.
/// Purpose: Bit manip helpers, based on CoreBoot header.

#define bk_set_bit(X, O)	  X = (1 << O) | X
#define bk_clear_bit(X, O)	  X = ~(1 << O) & X
#define bk_toogle(X, O)		  X = (1 << O) ^ X
#define bk_lsb(X)			  X = X & -X
#define bk_msb(X)			  X = -(mp_lsb(X)) & X
#define bk_look_for_bit(X, O) (1 << O) | X

#endif // ifndef __BITMANIP_H__
