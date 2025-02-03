#ifndef _BASETYPES_H_
#define _BASETYPES_H_

#include <windows.h>

typedef UINT64	size_64;
typedef UINT32	size_32;
typedef UINT16	size_16;
typedef UINT8	size_8;
typedef INT64	ssize_64;
typedef INT32	ssize_32;
typedef INT16	ssize_16;
typedef INT8	ssize_8;
typedef void*	ptr;

#ifdef _M_X64
typedef ssize_64	ptr_int;
typedef size_64		ptr_uint;
#else //_M_X64
typedef ssize_32	ptr_int;
typedef size_32		ptr_uint;
#endif //_M_X64

#define _32BIT_INVERSE(n) \
	( \
	((((size_32)(n)) & 0xFF000000) >> 24) | \
	((((size_32)(n)) & 0xFF0000) >> 8) | \
	((((size_32)(n)) & 0xFF00) << 8) | \
	((((size_32)(n)) & 0xFF) << 24) \
	)

#define _16BIT_INVERSE(n) \
	( \
	((((size_16)(n)) & 0xFF00) >> 8) | \
	((((size_16)(n)) & 0xFF) << 8) \
	)

#define SET_BIT(num, bit) ((num) |= (bit))
#define UNSET_BIT(num, bit) ((num) &= ~(bit))
#define TOGGLE_BIT(num, bit) ((num) ^= ((bit))
#define GET_BIT(num, bit) ((num) & (bit) ? 1 : 0)

#endif //_BASETYPES_H_
