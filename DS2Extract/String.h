#ifndef _STRING_H_
#define _STRING_H_

#include <stdio.h>
#include "Lists.h"
#include "BaseClasses.h"

class CStringW;
class CStringA;
class CStringBuffer;
class CStringBufferA;

class CStringA {
private:
	char*		m_pBuffer;
	static char	s_EmptyString[1];

	bool Alloc(int size);
	void Clear();
public:
	CStringA();
	CStringA(const CStringA &src);
	CStringA(const wchar_t* pSrc);
	CStringA(const char* pSrc);
	~CStringA();

	void Set(const char* pStr);
	void Print(const char* pFmt, ...);
	int _cdecl Scan(const char* pFmt, ...) const; //Implemented in StringxXX.asm
	const char* Get() const;
	operator const char* ()const{ return Get(); };
	int GetLength() const;
	void CopyFrom(const wchar_t* pSrc);
	const char* operator ()()const;
	const char* operator =(const char* pSrc);
	const char* operator =(const wchar_t* pSrc);
	CStringA& operator=(const CStringA &src);
	CStringA& operator=(const CStringBufferA &src);
	CStringA& operator=(const CStringW &src);
	CStringA& operator=(const CStringBuffer &src);

	CStringA Left(int count)const;
	CStringA Right(int count)const;
	CStringA Middle(int start, int count)const;

	bool operator==(const char* b)const;
	bool operator==(const CStringA &b)const;
	bool operator==(const wchar_t* b)const;
	bool operator==(const CStringW& b)const;

	bool operator!=(const char* b)const;
	bool operator!=(const CStringA &b)const;
	bool operator!=(const wchar_t* b)const;
	bool operator!=(const CStringW& b)const;
	
	//< is case insensitive equal, > unequal
	bool operator<(const char* b)const;
	bool operator<(const CStringA &b)const;
	bool operator<(const wchar_t* b)const;
	bool operator<(const CStringW& b)const;

	bool operator>(const char* b)const;
	bool operator>(const CStringA &b)const;
	bool operator>(const wchar_t* b)const;
	bool operator>(const CStringW& b)const;
};

class CStringW {
public:
	wchar_t*		m_pBuffer;
	static wchar_t	s_EmptyString[1];

	bool Alloc(int size);
	void Clear();
public:
	CStringW();
	CStringW(const CStringW &src);
	CStringW(const wchar_t* pSrc);
	CStringW(const char* pSrc);
	~CStringW();

	void Set(const wchar_t* pStr);
	void Print(const wchar_t* pFmt, ...);
	int _cdecl Scan(const wchar_t* pFmt, ...) const; //Implemented in StringxXX.asm
	const wchar_t* Get() const;
	operator const wchar_t* ()const{ return Get(); };
	int GetLength() const;
	void CopyFrom(const char* pSrc);
	const wchar_t* operator ()()const;
	const wchar_t* operator =(const char* pSrc);
	const wchar_t* operator =(const wchar_t* pSrc);
	CStringW& operator=(const CStringA &src);
	CStringW& operator=(const CStringBufferA &src);
	CStringW& operator=(const CStringW &src);
	CStringW& operator=(const CStringBuffer &src);

	CStringW Left(int count)const;
	CStringW Right(int count)const;
	CStringW Middle(int start, int count)const;

	bool operator==(const char* b)const;
	bool operator==(const CStringA &b)const;
	bool operator==(const wchar_t* b)const;
	bool operator==(const CStringW& b)const;
	
	bool operator!=(const char* b)const;
	bool operator!=(const CStringA &b)const;
	bool operator!=(const wchar_t* b)const;
	bool operator!=(const CStringW& b)const;
	
	bool operator<(const char* b)const;
	bool operator<(const CStringA &b)const;
	bool operator<(const wchar_t* b)const;
	bool operator<(const CStringW& b)const;

	bool operator>(const char* b)const;
	bool operator>(const CStringA &b)const;
	bool operator>(const wchar_t* b)const;
	bool operator>(const CStringW& b)const;
};

typedef CStringA CString;

template<typename T> class TStringCache {
public:
	template<typename T> class TRefStr : public T, public AutoPoolClass<TRefStr<T>, 64>{
	private:
		size_32	m_RefCount;
	public:
		TRefStr(){
			m_RefCount = 1;
		}
		virtual ~TRefStr(){
		}
		virtual size_32 AddRef(){
			return ++m_RefCount;
		}
		virtual size_32 Release(){
			if (--m_RefCount == 0){
				delete this;
				return 0;
			}
			return m_RefCount;
		}
	};// CRefStr;
	typedef TRefStr<T>	CRefStr;
private:
	static SList<typename CRefStr>			s_Available;
	static SList<typename CRefStr>			s_Used;
	static CriticalSectionClass		s_CS;
	static void ClearCallback(SList<CRefStr>*list, CRefStr* obj, void* cookie){
		obj->Release();
	}
public:
	TStringCache(){
		Initialize();
	}
	~TStringCache(){
		Shutdown();
	}
	static void Initialize(){
	}
	static void Shutdown(){
		s_CS.Enter();
		s_Available.Remove_All(ClearCallback);
		s_Used.Remove_All(ClearCallback);
		s_CS.Leave();
	}
	static CRefStr* Get(){
		CRefStr* str;
		GenericSLNode<CRefStr>* node;
		CriticalSectionClass::LockClass __lock(s_CS);
		if (node = s_Available.Get_Head()){
			str = node->m_pData;
			s_Available.Remove(str);
			s_Used.Add_Tail(str);
			str->AddRef();
			return str;
		}
		str = new CRefStr();
		if (!str)
			return false;
		s_Used.Add_Tail(str);
		str->AddRef();
		return str;
	}
	static void Return(CRefStr* str){
		CriticalSectionClass::LockClass __lock(s_CS);
		str->Release();
		if (!s_Used.Get_Head())
			return;
		s_Used.Remove(str);
		s_Available.Add_Tail(str);
	}
private:
	static TStringCache<T>	s_Instance;
};

template<typename T> SList<typename TStringCache<T>::CRefStr> TStringCache<T>::s_Available;
template<typename T> SList<typename TStringCache<T>::CRefStr> TStringCache<T>::s_Used;
template<typename T> CriticalSectionClass TStringCache<T>::s_CS;
template<typename T> TStringCache<T> TStringCache<T>::s_Instance;


typedef TStringCache<CStringA>	CStringCacheA;
typedef TStringCache<CStringW>	CStringCacheW;

#endif //!_STRING_H_
