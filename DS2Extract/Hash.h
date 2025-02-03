#ifndef _HASH_H_
#define _HASH_H_

#include "BaseTypes.h"
typedef UINT32 HASHKEY;

template<typename T, typename lf> HASHKEY GetHashT(const T *pStr, HASHKEY InVal, bool PostProcess){
	unsigned long res, k;
	int j;
	int len;
	len 
	T *s = (T*)pStr;
	len = lf(pStr);
	if (pStr && len){
		j = len & 3;
		res = initial_val;

		if ((len >>= 2)){
			do  {
				res += *s | (*(s+1)<<8);
				k = *(s+2) | (*(s+3)<<8);

				res += (res ^= ((k ^ (res << 5)) << 11)) >> 11;

				s += 4;
				len--;
			} while (len);
		}
		if (!(--j)){
			res += (*s);
			res += (res ^= (res << 10)) >> 1;
		} else if (!(--j)){
			res += *s | (*(s+1) << 8);
			res += (res ^= (res << 11)) >> 17;
		} else if (!(--j)){
			res += *s | (*(s+1) << 8 );
			res += (res ^= (((*(s+2) << 2) ^ res) << 16)) >> 11;
		}
	} else
		res = initial_val;
	if (post_process)
		res ^= (res += (res ^= (res += (res ^= res << 3) >> 5) << 2 ) >> 15) << 10;
	return res;
}

HASHKEY GetStrHash(const char *pStr, BOOL ins);
HASHKEY GetStrHashW(const wchar_t *pStr, BOOL ins);
HASHKEY GetHash(const unsigned char *pStr, int len, HASHKEY initial_val, BOOL post_process);
unsigned long GetHash(const char* pStr);

#endif //_HASH_H_
