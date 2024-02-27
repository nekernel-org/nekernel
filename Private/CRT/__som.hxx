/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __SOM__
#define __SOM__

typedef unsigned long long int _HandleCls;

/// @brief Looks like an objc class, because it takes inspiration from it.
typedef struct _Handle {
  _HandleCls _Cls;
  int (*Release)(_Handle *);
  _Handle* (*Sel)(_Handle*, ...);
} *Handle;

extern "C" Handle __som_send(int sel, int len, ...);

enum {
  kSomCreate,
  kSomQuery,
  kSomCount,
};

#define kHUnknown 11

#endif /* ifndef __SOM__ */