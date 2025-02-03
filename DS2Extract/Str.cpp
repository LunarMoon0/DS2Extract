#include <Windows.h>
#include "Str.h"
#include "StdLib.h"

wchar_t* PutUTF16(int _chr, wchar_t* _dst, int _len){
	if (_chr < 1 || _chr > WCH_UTF16_LAST || !_dst || _len < 1 || (_chr >= WCH_UTF16_HIGH_FIRST && _chr <= WCH_UTF16_LOW_LAST))
		return NULL;
	if ((_chr >= 0 && _chr < WCH_UTF16_HIGH_FIRST) || (_chr > WCH_UTF16_LOW_LAST && _chr < 0x10000)){
		*_dst = (wchar_t)_chr;
		return _dst+1;
	}
	if (_len < 2)
		return NULL;
	_chr -= 0x10000;
	*(_dst++) = WCH_UTF16_HIGH_FIRST+(_chr >> 10);
	*(_dst++) = WCH_UTF16_LOW_FIRST+(_chr & 0x3FF);
	return _dst;
}

int GetUTF16(const wchar_t* _str, int *_len, bool *_valid){
	int chr;
	if (!_str || !_valid || !_len)
		return 0;
	
	//Surrogate pair
	if (*_str >= WCH_UTF16_HIGH_FIRST && *_str <= WCH_UTF16_HIGH_LAST){
		chr = (*_str-WCH_UTF16_HIGH_FIRST) << 10;
		_str++;
		if (*_str < WCH_UTF16_LOW_FIRST || *_str > WCH_UTF16_LOW_LAST)
			return *_valid = false, *_len = 0, 0;
		chr = *_str-WCH_UTF16_LOW_FIRST+0x10000;
		*_len = 2;
		*_valid = true;
		return chr;
	}
	*_len = 1;
	*_valid = true;
	return *_str;
}

int GetUTF8(const char*_str, int *_len, bool *_valid){

	DWORD bit;
	unsigned char* pStr;
	DWORD i;
	int chr;
	if (!_str || !_valid)
		return 0;

	if (_len)
		*_len = 1;

	*_valid = false;

	pStr = (unsigned char*)_str;
	chr = *(pStr++);
	if (!(chr & UTF8FIRST)){
		*_valid = true;
		return chr;
	}
	if (!_BitScanReverse(&bit, ~(DWORD)(0xFFFFFF00 | chr)))
		return 0;
	chr &= (0xFF >> (8-bit));
	for (i = 0; i < (6-bit); i++, pStr++){
		if (!IsTrail(*pStr)){
			return 0;
		}
		chr = (chr << 6) | GetTrail(*pStr);
		if (_len)
			(*_len)++;
	}
	*_valid = true;
	return chr;
}

char* UTF8strchr(char *_str, int _chr){
	int ch;
	char *str;
	for (str = _str, ch = GetUTF8_i(_str); ch; str = _str, ch = GetUTF8_i(_str)){
		if (ch == _chr)
			return str;
	}
	return NULL;
}

const char* UTF8strchr(const char *_str, int _chr){
	int ch;
	const char *str;
	for (str = _str, ch = GetUTF8_i(_str); ch; str = _str, ch = GetUTF8_i(_str)){
		if (ch == _chr)
			return str;
	}
	return NULL;
}

char* BuildUTF8(char* _dst, int _chr){
	unsigned char* src;
	DWORD idx, i;
	int c;
	if (_chr < 0)
		return _dst;
	if (_chr < 128)
		return (*_dst = (char)_chr), _dst+1;
	if (!_BitScanReverse(&idx, (DWORD)_chr))
		return _dst;
	c = idx/6;
	idx = idx-(c*6);
	if ((2+c+1+idx) > 8){
		c++;
		idx = 0;
	}
	*(_dst++) = (0xFF << (8-(1 + c))) | ((_chr >> (c*6)) & (0xFF >> (1+c)));
	for (--c;c >= 0;c--){
		*(_dst++) = ((_chr >> (c*6)) & UTF8TRAILMASK) | UTF8TRAIL;
	}
	return _dst;
}

int GetUTF8ByteLen(int _chr){
	unsigned char* src;
	DWORD idx, i;
	int c;
	if (_chr < 0)
		return 0;
	if (_chr < 128)
		return 1;
	if (!_BitScanReverse(&idx, (DWORD)_chr))
		return 0;
	c = idx/6;
	idx = idx-(c*6);
	if ((2+c+1+idx) > 8){
		c++;
	}
	return 1+c;
}
