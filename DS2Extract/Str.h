#ifndef _STR_H_
#define _STR_H_
#include "StdLib.h"

#define WCH_UTF16_HIGH_FIRST    ((WCHAR)0xd800)
#define WCH_UTF16_HIGH_LAST     ((WCHAR)0xdbff)
#define WCH_UTF16_LOW_FIRST     ((WCHAR)0xdc00)
#define WCH_UTF16_LOW_LAST      ((WCHAR)0xdfff)
#define WCH_UTF16_LAST			((int)0x10FFFF)

#define UTF8TRAIL				(0x80)
#define UTF8FIRST				(0x80)
#define UTF8TRAILMASK			(0x3F)

#define GetTrail(_chr)			((_chr)&0x3F)
#define IsTrail(_chr)			(((_chr)&0xC0) == UTF8TRAIL)
#define IsUTF8har(_chr)			(((_chr)&UTF8FIRST) == UTF8FIRST)

wchar_t* PutUTF16(int _chr, wchar_t* _dst, int _len);
int GetUTF16(const wchar_t* _str, int *_len, bool *_valid);
int GetUTF8(const char*_str, int *_len, bool *_valid);
char* UTF8strchr(char *_str, int _chr);
const char* UTF8strchr(const char *_str, int _chr);
char* BuildUTF8(char* _dst, int _chr);
int GetUTF8ByteLen(int _chr);

inline int GetUTF8_i(char* &_str){
	int chr;
	bool valid;
	int len;
	chr = GetUTF8(_str, &len, &valid);
	_str += len;
	
	return valid ? chr : 0;
}

inline int GetUTF8_i(const char* &_str){
	int chr;
	bool valid;
	int len;
	chr = GetUTF8(_str, &len, &valid);
	_str += len;
	
	return valid ? chr : 0;
}

inline int GetUTF8l_i(char* &_str, int &_len){
	int chr;
	bool valid;
	chr = GetUTF8(_str, &_len, &valid);
	_str += _len;	
	return valid ? chr : 0;
}

inline int GetUTF8l_i(const char* &_str, int &_len){
	int chr;
	bool valid;
	chr = GetUTF8(_str, &_len, &valid);
	_str += _len;
	
	return valid ? chr : 0;
}



inline int PeekUTF8_i(const char* _str){
	int chr;
	bool valid;
	int len;
	chr = GetUTF8(_str, &len, &valid);	
	return valid ? chr : 0;
}

inline int PeekUTF8l_i(const char* _str, int *_len){
	int chr;
	bool valid;
	chr = GetUTF8(_str, _len, &valid);
	return valid ? chr : 0;
}

inline bool PutUTF8(char *_str, int _chr){
	char* _ptr;
	_ptr = BuildUTF8(_str, _chr);
	return _str != _ptr;
}

inline int GetUTF16_i(wchar_t* &_str){
	int len, chr;
	bool valid;
	chr = GetUTF16(_str, &len, &valid);
	_str += len;
	return valid ? chr : 0;
}

inline int GetUTF16_i(const wchar_t* &_str){
	int len, chr;
	bool valid;
	chr = GetUTF16(_str, &len, &valid);
	_str += len;
	return valid ? chr : 0;
}

inline int GetUTF16l_i(wchar_t* &_str, int &_len){
	int chr;
	bool valid;
	chr = GetUTF16(_str, &_len, &valid);
	_str += _len;
	return valid ? chr : 0;
}

inline int GetUTF16l_i(const wchar_t* &_str, int &_len){
	int chr;
	bool valid;
	chr = GetUTF16(_str, &_len, &valid);
	_str += _len;
	return valid ? chr : 0;
}


inline int PeekUTF16_i(wchar_t* _str){
	int len, chr;
	bool valid;
	chr = GetUTF16(_str, &len, &valid);
	return valid ? chr : 0;
}

inline int PeekUTF16_i(const wchar_t* _str){
	int len, chr;
	bool valid;
	chr = GetUTF16(_str, &len, &valid);
	return valid ? chr : 0;
}

inline int PeekUTF16l_i(wchar_t* _str, int &_len){
	int chr;
	bool valid;
	chr = GetUTF16(_str, &_len, &valid);
	return valid ? chr : 0;
}

inline int PeekUTF16l_i(const wchar_t* _str, int &_len){
	int chr;
	bool valid;
	chr = GetUTF16(_str, &_len, &valid);
	return valid ? chr : 0;
}

#endif //!_STR_H_
