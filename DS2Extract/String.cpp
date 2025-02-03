#include "String.h"
#include "StdLib.h"
#include "Memory.h"
#include "StringBuffer.h"

struct StringState {
	int	m_Allocated;
	int	m_Length;
};

#define GET_STATE_PTR(_ptr)			(StringState*)((unsigned char*)(_ptr)-sizeof(StringState))
#define GET_STR_PTR(_ptr)			((char*)(_ptr)+sizeof(StringState))
#define GET_STATE_LEN(_ptr)			(((StringState*)((unsigned char*)(_ptr)-sizeof(StringState)))->m_Length)
#define GET_STATE_ALLOCATED(_ptr)	(((StringState*)((unsigned char*)(_ptr)-sizeof(StringState)))->m_Allocated)

template<typename T> bool AllocString(T(*&_dst), int _len){
	StringState* pState;
	if (_dst){
		pState = GET_STATE_PTR(_dst);
		if (_len+1 < pState->m_Allocated){
			pState->m_Length = _len;
			return true;
		}
		g_pFreeer(pState);
		_dst =  NULL;
		if (_len == 0)
			return false;
	}
	pState =  (StringState*)g_pAllocator(sizeof(StringState)+((_len+1)*sizeof(T)));
	if (!pState){
		return false;
	}
	pState->m_Allocated = _len+1;
	pState->m_Length = _len;
	_dst = (T*)GET_STR_PTR(pState);
	return true;
}

template<typename T> void ClearString(T(*&_dst)){
	StringState* pState;
	pState = GET_STATE_PTR(_dst);
	g_pFreeer(pState);
	_dst = NULL;
}

/*
CStringA impl
*/

char CStringA::s_EmptyString[1] = "";

bool CStringA::Alloc(int size){
	if (m_pBuffer == s_EmptyString)
		m_pBuffer = NULL;
	if (!AllocString(m_pBuffer, size))
		return m_pBuffer = s_EmptyString, false;
	return true;
}

void CStringA::Clear(){
	if (m_pBuffer == s_EmptyString)
		return;
	ClearString(m_pBuffer);
	m_pBuffer = s_EmptyString;
}

CStringA::CStringA(){
	m_pBuffer = s_EmptyString;
}

CStringA::CStringA(const CStringA &src){
	m_pBuffer = s_EmptyString;
	*this = src;
}

CStringA::CStringA(const wchar_t* pSrc){
	m_pBuffer = s_EmptyString;
	*this = pSrc;
}

CStringA::CStringA(const char* pSrc){
	m_pBuffer = s_EmptyString;
	*this = pSrc;
}

CStringA::~CStringA(){
	Clear();
}

void CStringA::Set(const char* pStr){
	int len;
	if (!pStr){
		m_pBuffer = s_EmptyString;
		return;
	}
	len = strlen(pStr);
	if (!Alloc(len))
		return;
	memcpy_s(m_pBuffer, len+1, pStr, len+1);
}

void CStringA::Print(const char* pFmt, ...){
	va_list va;
	va_start(va, pFmt);
	int len;
	len = _vscprintf(pFmt, va);
	Clear();
	if (len <= 0)
		return;
	Alloc(len);
	vsprintf_s(m_pBuffer, len+1, pFmt, va);
}

/*int _cdecl CStringA::Scan(const char* pFmt, ...){
	va_list va;
	va_start(va, pFmt);

}*/

const char* CStringA::Get() const{
	return m_pBuffer;
}

int CStringA::GetLength() const {
	return (m_pBuffer == s_EmptyString) ? 0 : GET_STATE_LEN(m_pBuffer);
}

void CStringA::CopyFrom(const wchar_t* pSrc){
	int count;
	Clear();
	if (!pSrc || !(*pSrc))
		return;
	count = WideCharToMultiByte(CP_UTF8, 0, pSrc, -1, NULL, 0, NULL, NULL);
	if (!Alloc(count))
		return;	
	WideCharToMultiByte(CP_UTF8, 0, pSrc, -1, m_pBuffer, count, NULL, NULL);
}

const char* CStringA::operator ()()const{
	return m_pBuffer;
}

const char* CStringA::operator =(const char* pSrc){
	Set(pSrc);
	return m_pBuffer;
}

const char* CStringA::operator =(const wchar_t* pSrc){
	CopyFrom(pSrc);
	return m_pBuffer;
}

CStringA& CStringA::operator=(const CStringA &src){
	Set(src);
	return *this;
}

CStringA& CStringA::operator=(const CStringBufferA &src){
	Set(src);
	return *this;
}

CStringA& CStringA::operator=(const CStringW &src){
	//CopyFrom(src);
	return *this;
}

CStringA& CStringA::operator=(const CStringBuffer &src){
	CopyFrom(src);
	return *this;
}


CStringA CStringA::Left(int count)const{
	CStringA str;
	int len;
	if (count <= 0 || (len = GetLength()) == 0)
		return "";
	if (count > len)
		count = len;
	str.Print("%.*s", count, m_pBuffer);
	return str;
}

CStringA CStringA::Right(int count)const{
	int len;
	if (count <= 0 || (len = GetLength()) == 0)
		return "";
	if (count > len)
		count = len;
	return CStringA(m_pBuffer+(len-count));
}

CStringA CStringA::Middle(int start, int count)const{
	CStringA str;
	int len;
	len = GetLength();
	if (len == 0)
		return "";
	if (start+count > len)
		count = len-start;
	if (start < 0 || start > len || count < 0)
		return "";
	str.Print("%.*s", count, m_pBuffer+start);
	return str;
}


bool CStringA::operator==(const char* b)const{
	return (strcmp(m_pBuffer, b) == 0);
}

bool CStringA::operator==(const CStringA &b)const{
	return (strcmp(m_pBuffer, b) == 0);
}

bool CStringA::operator==(const wchar_t* b)const{
	CStringA buf;
	return (buf = b), (strcmp(m_pBuffer, buf) == 0);
}

bool CStringA::operator==(const CStringW& b)const{
	CStringA buf;
	return (buf = b), (strcmp(m_pBuffer, buf) == 0);
}

bool CStringA::operator!=(const char* b)const{
	return !(*this == b);
}

bool CStringA::operator!=(const CStringA &b)const{
	return !(*this == b);
}

bool CStringA::operator!=(const wchar_t* b)const{
	return !(*this == b);
}

bool CStringA::operator!=(const CStringW& b)const{
	return !(*this == b);
}


bool CStringA::operator<(const char* b)const{
	return (_stricmp(m_pBuffer, b) == 0);
}

bool CStringA::operator<(const CStringA &b)const{
	return (_stricmp(m_pBuffer, b) == 0);
}

bool CStringA::operator<(const wchar_t* b)const{
	CStringA buf;
	return (buf = b), (_stricmp(m_pBuffer, buf) == 0);
}

bool CStringA::operator<(const CStringW& b)const{
	CStringA buf;
	return (buf = b), (_stricmp(m_pBuffer, buf) == 0);
}

bool CStringA::operator>(const char* b)const{
	return !(*this < b);
}

bool CStringA::operator>(const CStringA &b)const{
	return !(*this < b);
}

bool CStringA::operator>(const wchar_t* b)const{
	return !(*this < b);
}

bool CStringA::operator>(const CStringW& b)const{
	return !(*this < b);
}

/*
CStringW impl
*/

wchar_t CStringW::s_EmptyString[1] = L"";

bool CStringW::Alloc(int size){
	if (m_pBuffer == s_EmptyString)
		m_pBuffer = NULL;
	if (!AllocString(m_pBuffer, size))
		return m_pBuffer = s_EmptyString, false;
	return true;
}

void CStringW::Clear(){
	if (m_pBuffer == s_EmptyString)
		return;
	ClearString(m_pBuffer);
	m_pBuffer = s_EmptyString;
}

CStringW::CStringW(){
	m_pBuffer = s_EmptyString;
}

CStringW::CStringW(const CStringW &src){
	m_pBuffer = s_EmptyString;
	*this = src;
}

CStringW::CStringW(const wchar_t* pSrc){
	m_pBuffer = s_EmptyString;
	*this = pSrc;
}

CStringW::CStringW(const char* pSrc){
	m_pBuffer = s_EmptyString;
	*this = pSrc;
}

CStringW::~CStringW(){
	Clear();
}

void CStringW::Set(const wchar_t* pStr){
	int len;
	if (!pStr){
		m_pBuffer = s_EmptyString;
		return;
	}
	len = wcslen(pStr);
	if (!Alloc(len))
		return;
	wcscpy_s(m_pBuffer, len+1, pStr);
}

void CStringW::Print(const wchar_t* pFmt, ...){
	va_list va;
	va_start(va, pFmt);
	int len;
	len = _vscwprintf(pFmt, va);
	Clear();
	if (len <= 0)
		return;
	Alloc(len);
	vswprintf_s(m_pBuffer, len+1, pFmt, va);
}

const wchar_t* CStringW::Get() const{
	return m_pBuffer;
}

int CStringW::GetLength() const {
	return (m_pBuffer == s_EmptyString) ? 0 : GET_STATE_LEN(m_pBuffer);
}

void CStringW::CopyFrom(const char* pSrc){
	int count;
	Clear();
	if (!pSrc || !(*pSrc))
		return;
	count = MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, NULL, 0);
	if (!Alloc(count))
		return;
	MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, m_pBuffer, count+1);
}

const wchar_t* CStringW::operator ()()const{
	return m_pBuffer;
}

const wchar_t* CStringW::operator =(const char* pSrc){
	CopyFrom(pSrc);
	return m_pBuffer;
}

const wchar_t* CStringW::operator =(const wchar_t* pSrc){
	Set(pSrc);
	return m_pBuffer;
}

CStringW& CStringW::operator=(const CStringA &src){
	CopyFrom(src);
	return *this;
}

CStringW& CStringW::operator=(const CStringBufferA &src){
	CopyFrom(src);
	return *this;
}

CStringW& CStringW::operator=(const CStringW &src){
	Set(src);
	return *this;
}

CStringW& CStringW::operator=(const CStringBuffer &src){
	Set(src);
	return *this;
}


CStringW CStringW::Left(int count)const{
	CStringW str;
	int len;
	if (count <= 0 || (len = GetLength()) == 0)
		return "";
	if (count > len)
		count = len;
	str.Print(L"%.*s", count, m_pBuffer);
	return str;
}

CStringW CStringW::Right(int count)const{
	int len;
	if (count <= 0 || (len = GetLength()) == 0)
		return "";
	if (count > len)
		count = len;
	return CStringW(m_pBuffer+(len-count));
}

CStringW CStringW::Middle(int start, int count)const{
	CStringW str;
	int len;
	len = GetLength();
	if (len == 0)
		return L"";
	if (start+count > len)
		count = len-start;
	if (start < 0 || start > len || count < 0)
		return L"";
	str.Print(L"%.*s", count, m_pBuffer+start);
	return str;
}


bool CStringW::operator==(const char* b)const{
	CStringW buf;
	return (buf = b), (wcscmp(m_pBuffer, buf) == 0);
}

bool CStringW::operator==(const CStringA &b)const{
	CStringW buf;
	return (buf = b), (wcscmp(m_pBuffer, buf) == 0);
}

bool CStringW::operator==(const wchar_t* b)const{
	return (wcscmp(m_pBuffer, b) == 0);
}

bool CStringW::operator==(const CStringW& b)const{
	return (wcscmp(m_pBuffer, b) == 0);
}

bool CStringW::operator!=(const char* b)const{
	return !(*this == b);
}

bool CStringW::operator!=(const CStringA &b)const{
	return !(*this == b);
}

bool CStringW::operator!=(const wchar_t* b)const{
	return !(*this == b);
}

bool CStringW::operator!=(const CStringW& b)const{
	return !(*this == b);
}


bool CStringW::operator<(const char* b)const{
	CStringW buf;
	return buf = b, (_wcsicmp(m_pBuffer, buf) == 0);
}

bool CStringW::operator<(const CStringA &b)const{
	CStringW buf;
	return buf = b, (_wcsicmp(m_pBuffer, buf) == 0);
}

bool CStringW::operator<(const wchar_t* b)const{
	return (_wcsicmp(m_pBuffer, b) == 0);
}

bool CStringW::operator<(const CStringW& b)const{
	return (_wcsicmp(m_pBuffer, b) == 0);
}

bool CStringW::operator>(const char* b)const{
	return !(*this < b);
}

bool CStringW::operator>(const CStringA &b)const{
	return !(*this < b);
}

bool CStringW::operator>(const wchar_t* b)const{
	return !(*this < b);
}

bool CStringW::operator>(const CStringW& b)const{
	return !(*this < b);
}
