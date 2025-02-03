#ifndef _STDLIB_H_
#define _STDLIB_H_
#include <windows.h>
#include "BaseTypes.h"

template<typename T> void qsort_t(T* _array, int _count, int (__cdecl*compare)(void* _ctx, T(*_elem0), T(*_elem1)), void* _ctx){
	qsort_s((void*)_array, _count, sizeof(T), (int (__cdecl*)(void*, const void*, const void*))compare, _ctx);
}

template<typename T, int _count> void qsort_t(T _array[_count], int (__cdecl*compare)(void* _ctx, const T*_elem0, const T*_elem1), void* _ctx){
	qsort_s((void*)_array, _count, sizeof(T), (int (__cdecl*)(void*, const void*, const void*))compare, _ctx);
}

template<typename T, typename TCTX> void qsort_tt(T* _array, int _count, int (__cdecl*compare)(TCTX* _ctx, const T*_elem0, const T*_elem1), TCTX* _ctx){
	qsort_s((void*)_array, _count, sizeof(T), (int (__cdecl*)(void*, const void*, const void*))compare, _ctx);
}

template<typename T, int _count, typename TCTX> void qsort_tt(T _array[_count], int (__cdecl*compare)(TCTX* _ctx, const T*_elem0, const T*_elem1), TCTX* _ctx){
	qsort_s((void*)_array, _count, sizeof(T), (int (__cdecl*)(void*, const void*, const void*))compare, _ctx);
}

int StrLen(const char *pStr);
char *StrCat2(char *pStr, const char *pAppend, BOOL ret_cur);
char *StrnCat2(char *pStr, const char *pAppend, int destsize, BOOL ret_cur);
char *StrCat(char *pStr, const char *pAppend);
char *StrnCat(char *pStr, const char *pAppend, int destsize);
const char *StrChr(const char *pStr, char Chr);
const char *StrrChr(const char *pStr, char Chr);
const char *StrStr(const char *pStr, const char *pFind);
const char *BinaryStr(const void *pBin, const char *pFind, size_32 size);

const char *StrCpy(char *pDest, const char *pSrc);
const char *StrnCpy(char *pDest, const char *pSrc, int destsize);
const char *StrnnCpy(char *pDest, const char *pSrc, int destsize, int srcsize);
char *StrDup(const char *pSrc);
char *StrnDup(const char *pStr, int len);

/*
Functions for filename string stuff
*/
char *Strip_File_Path(char *_filename);
char *Strip_Path_Parents(char *_path);
char *Remove_Slashes(char *pStr);
int Get_Path_Token_Count(const char *pPath);
char *Get_Path_Token(const char *pPath, int *pLength, int token, BOOL copy_token);
void Fix_Slashes(char *pPath);
int Get_Path_Token_Length(const char *pToken);
char *Format_Size(int size, char *buf, int bufsize);



/*
Unicode version
*/


int StrLenW(const wchar_t *pStr);
wchar_t *StrCat2W(wchar_t *pStr, const wchar_t *pAppend, BOOL ret_cur);
wchar_t *StrnCat2W(wchar_t *pStr, const wchar_t *pAppend, int destsize, BOOL ret_cur);
wchar_t *StrCatW(wchar_t *pStr, const wchar_t *pAppend);
wchar_t *StrnCatW(wchar_t *pStr, const wchar_t *pAppend, int destsize);
const wchar_t *StrChrW(const wchar_t *pStr, wchar_t Chr);
const wchar_t *StrrChrW(const wchar_t *pStr, wchar_t Chr);
const wchar_t *StrStrW(const wchar_t *pStr, const wchar_t *pFind);
const wchar_t *BinaryStrW(const void *pBin, const wchar_t *pFind, size_32 size);

const wchar_t *StrCpyW(wchar_t *pDest, const wchar_t *pSrc);
const wchar_t *StrnCpyW(wchar_t *pDest, const wchar_t *pSrc, int destsize);
const wchar_t *StrnnCpyW(wchar_t *pDest, const wchar_t *pSrc, int destsize, int srcsize);
wchar_t *StrDupW(const wchar_t *pSrc);
wchar_t *StrnDupW(const wchar_t *pStr, int len);

/*
Functions for filename string stuff
*/
wchar_t *Strip_File_PathW(wchar_t *_filename);
wchar_t *Strip_Path_ParentsW(wchar_t *_path);
wchar_t *Remove_SlashesW(wchar_t *pStr);
int Get_Path_Token_CountW(const wchar_t *pPath);
void Fix_SlashesW(wchar_t *pPath);
wchar_t *Get_Path_TokenW(const wchar_t *pPath, int *pLength, int token, BOOL copy_token);
int Get_Path_Token_LengthW(const wchar_t *pToken);

/*
File stuff
*/

#define file				HANDLE
#define INVALID_FILE		(INVALID_HANDLE_VALUE)
#define VALID_FILE(file)	((file) != INVALID_FILE)

bool fexists_(const char* filename);
bool fexists_W(const wchar_t* filename);
file fopen_(const char *_filename, const char *_mode);
file fopen_W(const wchar_t *_filename, const wchar_t *_mode);
void fclose_(file _file);
void fseteof_(file _file);

size_32 fread_(void *_buf, size_32 _ecount, file _file);
size_32 fwrite_(void *_buf, size_32 _count, file _file);

size_32 fseek_(file _file, LONG _offset, INT32 _origin);
size_32 ftell_(file _file);
size_32 fsize_(file _file);

bool feof_(file _file);

size_32 filecopy_(file _in, file _out, void *_cache, size_32 _cachesize, size_32 size);

template<typename T> bool fread_t(file f, T(*obj)){
	return (fread_(obj, sizeof(T), f) == sizeof(T));
}

/*
64 bit file support
*/

size_64 fseek_64(file _file, size_64 _offset, INT32 _origin);
size_64 fsize_64(file _file);
size_64 ftell_64(file _f);


/*
CRT ripped
*/

void * __cdecl MemMove (void *dst, const void *src, size_t count);
//void * __cdecl MemSet(void *dst, int val, size_t count);

void SkipTrailingWhitespace(const char* _start, const char* _end, char**_ptr, const char* _whitespace);
int TokenizeA(char *pSrc, int slen, char **DestArray, int MaxCount, const char* TokenChars = NULL, bool SkipLeadingWhiteSpace = true, bool SkipTrailingWhiteSpace = true, const char *WhiteSpace = " \t", const char *ExtraQuotation = NULL, bool TranslateSlashes = false, bool LeaveQuotation = false);
int cTokenizeA(const char *pSrc, int slen, const char* TokenChars = NULL, bool SkipLeadingWhiteSpace = true, bool SkipTrailingWhiteSpace = true, const char *WhiteSpace = " \t", const char *ExtraQuotation = NULL, bool TranslateSlashes = false, bool LeaveQuotation = false);

int TokenizeW(wchar_t *pSrc, wchar_t **dest_array, int max_count, const wchar_t *extra_ws, const wchar_t *extra_quotation, BOOL extended_whitespace, BOOL translate_slashes, BOOL leave_quotation, BOOL NoDefaultWhiteSpace);

void LogFormated(const wchar_t *fmt, ...);
void LogError();
void LogIID(const IID* pIID);
void MsgFormated(const wchar_t *fmt, ...);

char *Unicode_To_Multibyte(const wchar_t *unicode, int len = -1);
wchar_t *Multibyte_To_Unicode(const char *multibyte, int len = -1);
char* Unicode_To_Multibyte_Allocated(const wchar_t *unicode, int len = -1);
wchar_t* Multibyte_To_Unicode_Allocated(const char *multibyte, int len = -1);

bool MakeDirFromFilenameW(wchar_t* pBuf, const wchar_t *pBaseDir, const wchar_t *pFilename);

void InitializeStub(void *pReserved);

const char* GetPointerToParam(const char* pStr, int param);
const wchar_t* GetPointerToParamW(const wchar_t* pStr, int param);



#define DebugMessage	MsgFormated

template<typename T> inline void*memset_t(T &_Dst, int _Val){
	return memset(&_Dst, _Val, sizeof(T));
}

template<typename T> inline void*memset_at(T &_Dst, int _Val, int _Count){
	return memset(&_Dst, _Val, sizeof(T)*_Count);
}

template<typename T> inline void* zmem_t(T &_Dst){
	return memset(&_Dst, 0, sizeof(T));
}

template<typename T> inline void assign0_t(T &_Dst){
	_Dst = 0;
}



#endif //_STDLIB_H_
