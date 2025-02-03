#include "Memory.h"
#include "StringBuffer.h"
#include <string.h>
#include <wchar.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "Str.h"

/*
wchar_t version
*/

wchar_t CStringBuffer::s_EmptyString[1] = L"";

CStringBuffer::CStringBuffer(){
	m_Offset = 0;
	m_pBuffer = s_EmptyString;
	m_BufferSize = 0;
}

CStringBuffer::CStringBuffer(const CStringBuffer &src){
	m_Offset = 0;
	m_pBuffer = s_EmptyString;
	m_BufferSize = 0;
	*this = src;
}

CStringBuffer::CStringBuffer(const wchar_t* pSrc){
	m_Offset = 0;
	m_pBuffer = s_EmptyString;
	m_BufferSize = 0;
	*this = pSrc;
}

CStringBuffer::CStringBuffer(const char* pSrc){
	m_Offset = 0;
	m_pBuffer = s_EmptyString;
	m_BufferSize = 0;
	*this = pSrc;
}

CStringBuffer::~CStringBuffer(){
	Clear(true);
}

void CStringBuffer::Alloc(int size){
	int newsize;
	wchar_t* pTemp;
	if (size < m_BufferSize)
		return;
	newsize = ((size/_Align)+(size%_Align ? 1 : 0))*_Align;
	pTemp = (wchar_t*)g_pAllocator(sizeof(wchar_t)*newsize);
	//No check here
	if (m_pBuffer && m_pBuffer != s_EmptyString){
		wcscpy_s(pTemp, newsize, m_pBuffer);
		g_pFreeer(m_pBuffer);
	} else
		*pTemp = 0;
	m_pBuffer = pTemp;
	m_BufferSize = newsize;
}

int CStringBuffer::GetOffset(){
	return m_Offset;
}

void CStringBuffer::SetOffset(int offset){
	if (offset < 0)
		offset = 0;
	else if (offset >= m_BufferSize)
		offset = max(0, offset-1);
	m_Offset = offset;
}

int CStringBuffer::GetBufferSize(){
	return m_BufferSize;
}

void CStringBuffer::ZeroTerminate(){
	if (!m_pBuffer || m_pBuffer == s_EmptyString)
		return;
	if (m_pBuffer[m_Offset])
		return;
	if (m_Offset == m_BufferSize)
		Alloc(m_Offset+1);
	m_pBuffer[m_Offset] = 0;
}

int CStringBuffer::Append(const wchar_t* pString){
	int len;
	len = wcslen(pString);
	Alloc(m_Offset + len + 1);
	wcscpy_s(m_pBuffer+m_Offset, len + 1, pString);
	m_Offset += len;
	return m_Offset;
}

int CStringBuffer::AppendN(const wchar_t* pString, int len){
	int temp;
	temp = wcsnlen(pString, len);
	if (temp < len)
		len = temp;
	Alloc(m_Offset + len + 1);
	wmemcpy_s(m_pBuffer+m_Offset, len, pString, len);
	m_Offset += len;
	*(m_pBuffer+m_Offset) = 0;
	return m_Offset;
}

int CStringBuffer::Insert(const wchar_t* pString, int offset){
	int len;
	wchar_t *str, *str2;
	int dstcnt, cnt;
	if (offset >= m_Offset)
		return Append(pString);

	len = wcslen(pString);
	if (offset < 0){
		len += offset;
		if (len < 0)
			return m_Offset;
		pString += (len-offset)-len;
		offset = 0;
	}
	Alloc(m_Offset+len+1);
	str = m_pBuffer+offset+len;
	dstcnt = m_BufferSize-m_Offset+(m_Offset-offset);
	str2 = m_pBuffer+offset;
	cnt = m_Offset-offset;;
	wmemmove_s(str, dstcnt+1, str2, cnt+1);
	wmemcpy(m_pBuffer+offset, pString, len);
	m_Offset += len;
	return offset+len;
}

int CStringBuffer::OverWrite(const wchar_t* pString, int offset){
	int remain, len;
	if (offset == m_Offset)
		return Append(pString);
	else if (offset > m_Offset)
		return m_Offset;
	len = wcslen(pString);
	if (offset < 0){
		remain = 0-offset;
		len -= remain;
		if (len < 0)
			return 0;
		offset = 0;
	}
	Alloc(m_Offset-offset+len+1);
	remain = (m_Offset-offset < len) ? m_Offset-offset : len;
	wmemcpy_s(m_pBuffer+offset, len+1, pString, remain);
	if (len-remain > 0){
	len -= remain;
		wcscpy_s(m_pBuffer+m_Offset, m_BufferSize-m_Offset+(m_Offset-offset), pString);
		m_Offset += len;
		return m_Offset;
	} else
		return offset+len;
}

int CStringBuffer::Remove(int offset, int count){
	if (offset >= m_Offset)
		return m_Offset;
	if (offset < 0){
		count += offset;
		if (count <= 0)
			return m_Offset;
		offset = 0;
	}
	wmemmove_s(m_pBuffer+offset, m_BufferSize-offset, m_pBuffer+offset+count, m_Offset-offset-count+1);
	m_Offset -= count;
	return offset;
}

int CStringBuffer::Print(const wchar_t* pFmt, ...){
	va_list va;
	va_start(va, pFmt);
	return vPrint(pFmt, va);
}

int CStringBuffer::vPrint(const wchar_t* pFmt, va_list va){
	int len;
	len = _vscwprintf(pFmt, va);
	if (len <= 0)
		return m_Offset;
	Alloc(len + 1);
	vswprintf_s(m_pBuffer, len+1, pFmt, va);
	m_Offset = len;
	return len;
}

void CStringBuffer::Clear(bool freedata){
	if (freedata){
		if (m_pBuffer && m_pBuffer != s_EmptyString)
			g_pFreeer(m_pBuffer);
		m_BufferSize = 0;
		m_pBuffer = s_EmptyString;
	} else if (m_pBuffer)
		m_pBuffer[0] = 0;

	m_Offset = 0;
}

const wchar_t* CStringBuffer::GetString() const{
	return m_pBuffer;
}

wchar_t* CStringBuffer::GetBufferPtr(){
	return m_pBuffer;
}

int CStringBuffer::CopyFrom(const char* pSrc){
	int count;
	Clear();
	if (!pSrc || !(*pSrc))
		return 0;
	count = MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, NULL, 0);
	Alloc(count+1);
	MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, m_pBuffer, m_BufferSize);
	return m_Offset = count;
}

int CStringBuffer::AppenChr(int chr){
	wchar_t lBuf[3];
	wchar_t* _ptr;
	_ptr = PutUTF16(chr, lBuf, 3);
	if (_ptr == lBuf)
		return m_Offset;
	Alloc(m_Offset+1+(_ptr-lBuf));
	wmemcpy_s(m_pBuffer+m_Offset, _ptr-lBuf, lBuf, _ptr-lBuf);
	m_Offset += _ptr-lBuf;
	m_pBuffer[m_Offset] = 0;
	return m_Offset;
}

int CStringBuffer::AppendF(const wchar_t* pFmt, ...){
	va_list va;
	va_start(va, pFmt);
	return vAppendF(pFmt, va);
}

int CStringBuffer::vAppendF(const wchar_t* pFmt, va_list va){
	int len;
	len = _vscwprintf(pFmt, va);
	if (len <= 0)
		return m_Offset;
	Alloc(m_Offset + len + 1);
	vswprintf_s(m_pBuffer+m_Offset, len+1, pFmt, va);
	m_Offset += len;
	return m_Offset;
}

int CStringBuffer::InsertF(const wchar_t* pFmt, int offset, ...){
	va_list va;
	va_start(va, pFmt);
	return vInsertF(pFmt, offset, va);
}

int CStringBuffer::vInsertF(const wchar_t* pFmt, int offset, va_list va){
	CStringBuffer buf;
	buf.vAppendF(pFmt, va);
	return Insert(buf, offset);
}

int CStringBuffer::OverwriteF(const wchar_t* pFmt, int offset, ...){
	va_list va;
	va_start(va, pFmt);
	return vInsertF(pFmt, offset, va);
}

int CStringBuffer::vOverwriteF(const wchar_t* pFmt, int offset, va_list va){
	CStringBuffer buf;
	buf.vAppendF(pFmt, va);
	return OverWrite(buf, offset);
}

void CStringBuffer::Set(const wchar_t* pStr){
	int len;
	if (!pStr){
		Clear();
		return;
	}
	len = wcslen(pStr)+1;
	Alloc(len);
	if (m_pBuffer)
		wcscpy_s(m_pBuffer, m_BufferSize, pStr);
}

wchar_t* CStringBuffer::operator ()()const{
	return m_pBuffer;
}

wchar_t* CStringBuffer::operator =(const wchar_t* pSrc){
	Set(pSrc);
	return m_pBuffer;
}

wchar_t* CStringBuffer::operator =(const char* pSrc){
	CopyFrom(pSrc);
	return m_pBuffer;
}

CStringBuffer& CStringBuffer::operator=(const CStringBuffer &src){
	Set(src.GetString());
	return *this;
}

CStringBuffer& CStringBuffer::operator=(const CStringBufferA &src){
	CopyFrom(src.GetString());
	return *this;
}

CStringBuffer CStringBuffer::Left(int count)const{
	int len;
	CStringBuffer ret;
	if (count < 0 || (len = (int)wcslen(m_pBuffer)) == 0)
		return L"";

	if (count > len)
		count = len;

	ret.AppendF(L"%.*s", count, m_pBuffer);
	return ret;
}

CStringBuffer CStringBuffer::Right(int count)const{
	int len;
	if (count < 0 || (len = (int)wcslen(m_pBuffer)) == 0)
		return L"";

	if (count > len)
		count = len;

	return CStringBuffer(m_pBuffer+(len-count));
}

CStringBuffer CStringBuffer::Middle(int start, int count)const{
	int len;
	CStringBuffer ret;
	if ((len = (int)wcslen(m_pBuffer)) == 0)
		return L"";
	if (start < 0 || start > len || count < 0)
		return L"";

	if (start+count > len)
		count = len-start;
	
	ret.AppendF(L"%.*s", count, m_pBuffer+start);
	return ret;
}

bool CStringBuffer::operator==(const wchar_t* b)const{
	return (wcscmp(m_pBuffer, b) == 0);
}

bool CStringBuffer::operator==(const CStringBuffer &b)const{
	return (wcscmp(m_pBuffer, b) == 0);
}

bool CStringBuffer::operator==(const char* b)const{
	CStringBuffer buf;
	return (buf = b), (wcscmp(m_pBuffer, buf) == 0);
}

bool CStringBuffer::operator==(const CStringBufferA& b)const{
	CStringBuffer buf;
	return (buf = b), (wcscmp(m_pBuffer, buf) == 0);
}

/*
char version
*/

char CStringBufferA::s_EmptyString[1] = "";

CStringBufferA::CStringBufferA(){
	m_Offset = 0;
	m_pBuffer = s_EmptyString;
	m_BufferSize = 0;
}

CStringBufferA::CStringBufferA(const CStringBufferA &src){
	m_Offset = 0;
	m_pBuffer = s_EmptyString;
	m_BufferSize = 0;
	*this = src;
}

CStringBufferA::CStringBufferA(const char* pSrc){
	m_Offset = 0;
	m_pBuffer = s_EmptyString;
	m_BufferSize = 0;
	*this = pSrc;
}

CStringBufferA::CStringBufferA(const wchar_t* pSrc){
	m_Offset = 0;
	m_pBuffer = s_EmptyString;
	m_BufferSize = 0;
	*this = pSrc;
}

CStringBufferA::~CStringBufferA(){
	Clear(true);
}

void CStringBufferA::Alloc(int size){
	int newsize;
	char* pTemp;
	//LogFormated(L"CStringBufferA::Alloc -> requested size: %d, current buffer size: %d\n", size, m_BufferSize);
	if (size <= m_BufferSize)
		return;
	newsize = ((size/_Align)+(size%_Align ? 1 : 0))*_Align;
	pTemp = (char*)g_pAllocator(sizeof(char)*newsize);
	//No check here
	if (m_pBuffer && m_pBuffer != s_EmptyString){
		strcpy_s(pTemp, sizeof(char)*newsize, m_pBuffer);
		g_pFreeer(m_pBuffer);
	} else
		*pTemp = 0;
	m_pBuffer = pTemp;
	m_BufferSize = newsize;
}

int CStringBufferA::GetOffset(){
	return m_Offset;
}

void CStringBufferA::SetOffset(int offset){
	if (offset < 0)
		offset = 0;
	else if (offset >= m_BufferSize)
		offset = max(0, offset-1);
	m_Offset = offset;
}

int CStringBufferA::GetBufferSize(){
	return m_BufferSize;
}

void CStringBufferA::ZeroTerminate(){
	if (!m_pBuffer || m_pBuffer == s_EmptyString)
		return;
	if (!m_pBuffer[m_Offset])
		return;
	if (m_Offset == m_BufferSize)
		Alloc(m_Offset+1);
	m_pBuffer[m_Offset] = 0;
}

int CStringBufferA::Append(const char* pString){
	int len;
	len = strlen(pString);
	Alloc(m_Offset + len + 1);
	strcpy_s(m_pBuffer+m_Offset, len + 1, pString);
	m_Offset += len;
	return m_Offset;
}

int CStringBufferA::AppendN(const char* pString, int len){
	int temp;
	temp = strnlen(pString, len);
	if (temp < len)
		len = temp;
	Alloc(m_Offset + len + 1);
	memcpy_s(m_pBuffer+m_Offset, len, pString, len);
	m_Offset += len;
	*(m_pBuffer+m_Offset) = 0;
	return m_Offset;
}

int CStringBufferA::Insert(const char* pString, int offset){
	int len;
	char *str, *str2;
	int dstcnt, cnt;
	if (offset >= m_Offset)
		return Append(pString);

	len = strlen(pString);
	if (offset < 0){
		len += offset;
		if (len < 0)
			return m_Offset;
		pString += (len-offset)-len;
		offset = 0;
	}
	Alloc(m_Offset+len+1);
	str = m_pBuffer+offset+len;
	dstcnt = m_BufferSize-m_Offset+(m_Offset-offset);
	str2 = m_pBuffer+offset;
	cnt = m_Offset-offset;;
	memmove_s(str, dstcnt, str2, cnt);
	memcpy(m_pBuffer+offset, pString, len);
	m_Offset += len;
	return offset+len;
}

int CStringBufferA::OverWrite(const char* pString, int offset){
	int remain, len;
	if (offset == m_Offset)
		return Append(pString);
	else if (offset > m_Offset)
		return m_Offset;
	len = strlen(pString);
	if (offset < 0){
		remain = 0-offset;
		len -= remain;
		if (len < 0)
			return 0;
		offset = 0;
	}
	Alloc(m_Offset-offset+len+1);
	remain = (m_Offset-offset < len) ? m_Offset-offset : len;
	memcpy_s(m_pBuffer+offset, len+1, pString, remain);
	if (len-remain > 0){
		len -= remain;
		strcpy_s(m_pBuffer+m_Offset, m_BufferSize-m_Offset+(m_Offset-offset), pString);
		m_Offset += len;
		return m_Offset;
	} else
		return offset+len;
}

int CStringBufferA::Print(const char* pFmt, ...){
	va_list va;
	va_start(va, pFmt);
	return vPrint(pFmt, va);
}

int CStringBufferA::vPrint(const char* pFmt, va_list va){
	int len;
	len = _vscprintf(pFmt, va);
	if (len <= 0)
		return Clear(), m_Offset;
	Alloc(len + 1);
	vsprintf_s(m_pBuffer, len+1, pFmt, va);
	m_Offset = len;
	return len;
}

int CStringBufferA::Remove(int offset, int count){
	if (offset >= m_Offset)
		return m_Offset;
	if (offset < 0){
		count += offset;
		if (count <= 0)
			return m_Offset;
		offset = 0;
	}
	memmove_s(m_pBuffer+offset, m_BufferSize-offset, m_pBuffer+offset+count, m_Offset-offset-count+1);
	m_Offset -= count;
	return offset;
}

void CStringBufferA::Clear(bool freedata){
	if (freedata){
		if (m_pBuffer && m_pBuffer != s_EmptyString)
			g_pFreeer(m_pBuffer);
		m_BufferSize = 0;
		m_pBuffer = s_EmptyString;
	} else if (m_pBuffer)
		m_pBuffer[0] = 0;

	m_Offset = 0;
}

const char* CStringBufferA::GetString() const{
	return m_pBuffer ? m_pBuffer : "";
}

char* CStringBufferA::GetBufferPtr(){
	return m_pBuffer;
}

int CStringBufferA::CopyFrom(const wchar_t* pSrc){
	int count;
	Clear();
	if (!pSrc || !(*pSrc))
		return 0;
	count = WideCharToMultiByte(CP_UTF8, 0, pSrc, -1, NULL, 0, NULL, NULL);
	Alloc(count+1);
	WideCharToMultiByte(CP_UTF8, 0, pSrc, -1, m_pBuffer, m_BufferSize, NULL, NULL); 
	return m_Offset = count;
}

int CStringBufferA::AppenChr(int chr){
	char lBuf[16];
	char* _ptr;
	_ptr = BuildUTF8(lBuf, chr);
	if (_ptr == lBuf)
		return m_Offset;
	Alloc(m_Offset+1+(_ptr-lBuf));
	memcpy(m_pBuffer+m_Offset, lBuf, _ptr-lBuf);
	m_Offset += _ptr-lBuf;
	m_pBuffer[m_Offset] = 0;
	return m_Offset;
}

int CStringBufferA::AppendF(const char* pFmt, ...){
	va_list va;
	va_start(va, pFmt);
	return vAppendF(pFmt, va);
}

int CStringBufferA::vAppendF(const char* pFmt, va_list va){	
	int len;
	len = _vscprintf(pFmt, va);
	if (len <= 0)
		return m_Offset;
	Alloc(m_Offset + len + 1);
	vsprintf_s(m_pBuffer+m_Offset, len+1, pFmt, va);
	m_Offset += len;
	return m_Offset;
}

int CStringBufferA::InsertF(const char* pFmt, int offset, ...){
	va_list va;
	va_start(va, pFmt);
	return vInsertF(pFmt, offset, va);
}

int CStringBufferA::vInsertF(const char* pFmt, int offset, va_list va){
	CStringBufferA buf;
	buf.vAppendF(pFmt, va);
	return Insert(buf, offset);
}

int CStringBufferA::OverwriteF(const char* pFmt, int offset, ...){
	va_list va;
	va_start(va, pFmt);
	return vInsertF(pFmt, offset, va);
}

int CStringBufferA::vOverwriteF(const char* pFmt, int offset, va_list va){
	CStringBufferA buf;
	buf.vAppendF(pFmt, va);
	return OverWrite(buf, offset);
}


void CStringBufferA::Set(const char* pStr){
	int len;
	if (!pStr){
		Clear();
		return;
	}
	len = strlen(pStr)+1;
	Alloc(len);
	if (m_pBuffer)
		strcpy_s(m_pBuffer, m_Offset=len, pStr);
}

char* CStringBufferA::operator ()()const{
	return m_pBuffer;
}

char* CStringBufferA::operator =(const char* pSrc){
	Set(pSrc);
	return m_pBuffer;
}

char* CStringBufferA::operator =(const wchar_t* pSrc){
	CopyFrom(pSrc);
	return m_pBuffer;
}

CStringBufferA& CStringBufferA::operator=(const CStringBufferA &src){
	Set(src.GetString());
	return *this;
}

CStringBufferA& CStringBufferA::operator=(const CStringBuffer &src){
	CopyFrom(src.GetString());
	return *this;
}

CStringBufferA CStringBufferA::Left(int count)const{
	int len;
	CStringBufferA ret;
	if (count < 0 || (len = (int)strlen(m_pBuffer)) == 0)
		return "";

	if (count > len)
		count = len;

	ret.AppendF("%.*s", count, m_pBuffer);
	return ret;
}

CStringBufferA CStringBufferA::Right(int count)const{
	int len;
	if (count < 0 || (len = (int)strlen(m_pBuffer)) == 0)
		return L"";

	if (count > len)
		count = len;

	return CStringBufferA(m_pBuffer+(len-count));
}

CStringBufferA CStringBufferA::Middle(int start, int count)const{
	int len;
	CStringBufferA ret;
	if ((len = (int)strlen(m_pBuffer)) == 0)
		return L"";
	if (start < 0 || start > len || count < 0)
		return L"";

	if (start+count > len)
		count = len-start;
	
	ret.AppendF("%.*s", count, m_pBuffer+start);
	return ret;
}

bool CStringBufferA::operator==(const char* b)const{
	return (strcmp(m_pBuffer, b) == 0);
}

bool CStringBufferA::operator==(const CStringBufferA &b)const{
	return (strcmp(m_pBuffer, b) == 0);
}

bool CStringBufferA::operator==(const wchar_t* b)const{
	CStringBufferA buf;
	return (buf = b), (strcmp(m_pBuffer, buf) == 0);
}

bool CStringBufferA::operator==(const CStringBuffer& b)const{
	CStringBufferA buf;
	return (buf = b), (strcmp(m_pBuffer, buf) == 0);
}
