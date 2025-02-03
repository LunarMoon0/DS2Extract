#ifndef _STRINGBUFFER_H_
#define _STRINGBUFFER_H_

#include <stdio.h>

class CStringBuffer;
class CStringBufferA;

class CStringBuffer {
protected:
	wchar_t*	m_pBuffer;
	int			m_BufferSize;
	int			m_Offset;
	enum {
		_Align = 256,
	};
	static	wchar_t s_EmptyString[1];
public:
	CStringBuffer();
	CStringBuffer(const CStringBuffer &src);
	CStringBuffer(const wchar_t* pSrc);
	CStringBuffer(const char* pSrc);
	~CStringBuffer();
	virtual void Alloc(int size);
	int GetOffset();
	void SetOffset(int offset);
	int GetBufferSize();
	void ZeroTerminate();
	int Append(const wchar_t* pString);
	int AppendN(const wchar_t* pString, int len);
	int Insert(const wchar_t* pString, int offset);
	int OverWrite(const wchar_t* pString, int offset);
	int Print(const wchar_t* pFmt, ...);
	int vPrint(const wchar_t* pFmt, va_list va);
	int Remove(int offset, int count);
	virtual void Clear(bool freedata = false);
	const wchar_t* GetString() const;
	wchar_t* GetBufferPtr();
	operator const wchar_t* () const { return GetString(); };
	int CopyFrom(const char* pSrc);
	int AppenChr(int chr);
	int AppendF(const wchar_t* pFmt, ...);
	int vAppendF(const wchar_t* pFmt, va_list va);
	int InsertF(const wchar_t* pFmt, int offset, ...);
	int vInsertF(const wchar_t* pFmt, int offset, va_list va);
	int OverwriteF(const wchar_t* pFmt, int offset, ...);
	int vOverwriteF(const wchar_t* pFmt, int offset, va_list va);
	void Set(const wchar_t* pStr);
	wchar_t* operator ()()const;
	wchar_t* operator =(const wchar_t* pSrc);
	wchar_t* operator =(const char* pSrc);
	CStringBuffer& operator=(const CStringBuffer &src);
	CStringBuffer& operator=(const CStringBufferA &src);

	CStringBuffer Left(int count)const;
	CStringBuffer Right(int count)const;
	CStringBuffer Middle(int start, int count)const;

	bool operator==(const wchar_t* b)const;
	bool operator==(const CStringBuffer &b)const;
	bool operator==(const char* b)const;
	bool operator==(const CStringBufferA& b)const;
};

class CStringBufferA {
protected:
	char*	m_pBuffer;
	int		m_BufferSize;
	int		m_Offset;
	enum {
		_Align = 256,
	};
	static	char s_EmptyString[1];
public:
	CStringBufferA();
	CStringBufferA(const CStringBufferA &src);
	CStringBufferA(const char* pSrc);
	CStringBufferA(const wchar_t* pSrc);
	~CStringBufferA();
	virtual void Alloc(int size);
	int GetOffset();
	void SetOffset(int offset);
	int GetBufferSize();
	void ZeroTerminate();
	int Append(const char* pString);
	int AppendN(const char* pString, int len);
	int Insert(const char* pString, int offset);
	int OverWrite(const char* pString, int offset);
	int Print(const char* pFmt, ...);
	int vPrint(const char* pFmt, va_list va);
	int Remove(int offset, int count);
	virtual void Clear(bool freedata = false);
	const char* GetString() const;
	char* GetBufferPtr();
	operator const char* ()const{ return GetString(); };
	int CopyFrom(const wchar_t* pSrc);
	int AppenChr(int chr);
	int AppendF(const char* pFmt, ...);
	int vAppendF(const char* pFmt, va_list va);
	int InsertF(const char* pFmt, int offset, ...);
	int vInsertF(const char* pFmt, int offset, va_list va);
	int OverwriteF(const char* pFmt, int offset, ...);
	int vOverwriteF(const char* pFmt, int offset, va_list va);
	void Set(const char* pStr);
	char* operator ()()const;
	char* operator =(const char* pSrc);
	char* operator =(const wchar_t* pSrc);
	CStringBufferA& operator=(const CStringBufferA &src);
	CStringBufferA& operator=(const CStringBuffer &src);

	CStringBufferA Left(int count)const;
	CStringBufferA Right(int count)const;
	CStringBufferA Middle(int start, int count)const;

	bool operator==(const char* b)const;
	bool operator==(const CStringBufferA& b)const;
	bool operator==(const wchar_t* b)const;
	bool operator==(const CStringBuffer &b)const;
};

template<int _bufsize> class TStringBufferA : public CStringBufferA {
protected:
	char	m_Buffer[_bufsize];
public:
	TStringBufferA(){
	}
	TStringBufferA(const CStringBufferA &src) : CStringBufferA(src){
	}

	TStringBufferA(const char* pSrc) : CStringBufferA(pSrc){
	}
	TStringBufferA(const wchar_t* pSrc) : CStringBufferA(pSrc){
	}
	~TStringBufferA(){
		Clear(true);
	}
	
	virtual void Alloc(int size){
		if (size > _bufsize || (m_pBuffer != m_Buffer && m_pBuffer != s_EmptyString))
			return CStringBufferA::Alloc(size);
		m_pBuffer = m_Buffer;
		if (m_Offset == 0)
			m_Buffer[0] = 0;
		m_BufferSize = size;
	}
	virtual void Clear(bool freedata = false){
		if (m_pBuffer != m_Buffer)
			CStringBufferA::Clear(freedata);
		else {
			m_pBuffer = s_EmptyString;
			m_BufferSize = 0;
			m_Offset = 0;
		}
	}
	char* operator =(const char* pSrc){
		Set(pSrc);
		return (char*)((*this)());
	}
	char* operator =(const wchar_t* pSrc){
		CopyFrom(pSrc);
		return (char*)((*this)());
	}
	TStringBufferA& operator=(const CStringBufferA &src){
		Set(src);
		return (*this);
	}
	TStringBufferA& operator=(const CStringBuffer &src){
		CopyFrom(src);
		return (*this);
	}
	template<int _tbufsize> TStringBufferA<_tbufsize> &Left(TStringBufferA<_tbufsize> &ret, int count)const{
		int len;
		if (count < 0 || (len = (int)strlen(m_pBuffer)) == 0)
			return ret.Set(""), ret;

		if (count > len)
			count = len;

		ret.Print("%.*s", count, m_pBuffer);
		return ret;
	}

	template<int _tbufsize> TStringBufferA<_tbufsize> &Right(TStringBufferA<_tbufsize> &ret, int count)const{
		int len;
		if (count < 0 || (len = (int)strlen(m_pBuffer)) == 0)
			return ret.Set(""), ret;

		if (count > len)
			count = len;

		return ret.Set(m_pBuffer+(len-count)), ret;
	}

	template<int _tbufsize> TStringBufferA<_tbufsize> &Middle(TStringBufferA<_tbufsize> &ret, int start, int count)const{
		int len;
		if ((len = (int)strlen(m_pBuffer)) == 0)
			return ret.Set(""), ret;
		if (start < 0 || start > len || count < 0)
			return ret.Set(""), ret;

		if (start+count > len)
			count = len-start;
	
		ret.Print("%.*s", count, m_pBuffer+start);
		return ret;
	}
};

template<int _bufsize> class TStringBufferW : public CStringBuffer {
protected:
	wchar_t	m_Buffer[_bufsize];
public:
	TStringBufferW(){
	}
	TStringBufferW(const CStringBuffer &src) : CStringBuffer(src){
	}

	TStringBufferW(const char* pSrc) : CStringBuffer(pSrc){
	}
	TStringBufferW(const wchar_t* pSrc) : CStringBuffer(pSrc){
	}
	~TStringBufferW(){
		Clear(true);
	}
	
	virtual void Alloc(int size){
		if (size > _bufsize || (m_pBuffer != m_Buffer && m_pBuffer != s_EmptyString))
			return CStringBuffer::Alloc(size);
		m_pBuffer = m_Buffer;
		if (m_Offset == 0)
			m_Buffer[0] = 0;
		m_BufferSize = size;
	}
	virtual void Clear(bool freedata = false){
		if (m_pBuffer != m_Buffer)
			CStringBuffer::Clear(freedata);
		else {
			m_pBuffer = s_EmptyString;
			m_BufferSize = 0;
			m_Offset = 0;
		}
	}
	char* operator =(const char* pSrc){
		CopyFrom(pSrc);
		return (char*)((*this)());
	}
	char* operator =(const wchar_t* pSrc){
		Set(pSrc);
		return (char*)((*this)());
	}
	TStringBufferW& operator=(const CStringBufferA &src){
		CopyFrom(src);
		return (*this);
	}
	TStringBufferW& operator=(const CStringBuffer &src){
		Set(src);
		return (*this);
	}
	template<int _tbufsize> TStringBufferW<_tbufsize> &Left(TStringBufferW<_tbufsize> &ret, int count)const{
		int len;
		if (count < 0 || (len = (int)strlen(m_pBuffer)) == 0)
			return ret.Set(L""), ret;

		if (count > len)
			count = len;

		ret.Print(L"%.*s", count, m_pBuffer);
		return ret;
	}

	template<int _tbufsize> TStringBufferW<_tbufsize> &Right(TStringBufferW<_tbufsize> &ret, int count)const{
		int len;
		if (count < 0 || (len = (int)wcslen(m_pBuffer)) == 0)
			return ret.Set(L""), ret;

		if (count > len)
			count = len;

		return ret.Set(m_pBuffer+(len-count)), ret;
	}

	template<int _tbufsize> TStringBufferW<_tbufsize> &Middle(TStringBufferW<_tbufsize> &ret, int start, int count)const{
		int len;
		if ((len = (int)strlen(m_pBuffer)) == 0)
			return ret.Set(L""), ret;
		if (start < 0 || start > len || count < 0)
			return ret.Set(L""), ret;

		if (start+count > len)
			count = len-start;
	
		ret.Print(L"%.*s", count, m_pBuffer+start);
		return ret;
	}
};

#endif //!_STRINGBUFFER_H_
