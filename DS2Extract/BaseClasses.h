#ifndef _BASECLASSES_H_
#define _BASECLASSES_H_

#include <windows.h>
#include "Base.h"

interface IConstUnknown {
public:
	virtual ULONG AddRef() = 0;
	virtual ULONG Release() = 0;
};

#define ECS(cs) (EnterCriticalSection((cs)))
#define LCS(cs) (LeaveCriticalSection((cs)))
#define TCS(cs) (TryEnterCriticalSection((cs)))
#define KTCS(cs) if (!TryEnterCriticalSection((cs))) EnterCriticalSection((cs))
#define ICS(cs) (InitializeCriticalSection((cs)))
#define DCS(cs) (DeleteCriticalSection((cs)))

/*
CriticalSectionClass
*/

class CriticalSectionClass {
private:
	CRITICAL_SECTION m_CS;
public:
	SHARED_INLINE CriticalSectionClass(void);
	SHARED_INLINE ~CriticalSectionClass(void);
	class LockClass {
	private:
		CriticalSectionClass *m_pCS;
	public:
		SHARED_INLINE LockClass(CriticalSectionClass &pSection);
		SHARED_INLINE ~LockClass(void);
	};
	SHARED_INLINE void Enter(void);
	SHARED_INLINE void Leave(void);
};

/*
EventClass
*/

class EventClass {
private:
	HANDLE m_Event;
public:
	SHARED_INLINE EventClass(void);
	SHARED_INLINE EventClass(bool bManualReset);
	SHARED_INLINE void Open(const char* pName, bool all = false);
	SHARED_INLINE ~EventClass(void);
	SHARED_INLINE void Set(void);
	SHARED_INLINE void Reset(void);
	SHARED_INLINE bool Wait(unsigned long msTimeout = INFINITE); //Returns true if timed out; use INFINITE to wait indefinitely
	SHARED_INLINE HANDLE GetHandle();
	SHARED_INLINE void AssignHandle(HANDLE hEvent);
};

/*
ExternEventClass
*/

class ExternEventClass {
private:
	HANDLE m_Event;
public:
	SHARED_INLINE ExternEventClass();
	SHARED_INLINE ~ExternEventClass();
	SHARED_INLINE void SetHandle(HANDLE hEvent);
	SHARED_INLINE void Set();
	SHARED_INLINE void Reset();
	SHARED_INLINE bool Wait(unsigned long msTimeout = INFINITE); //Returns true if timed out; use INFINITE to wait indefinitely
};

/*
NamedEventClass
*/

class NamedEventClass {
private:
	HANDLE m_hEvent;
public:
	SHARED_INLINE NamedEventClass(void);
	SHARED_INLINE ~NamedEventClass(void);
	SHARED_INLINE void Create(const char* pName, bool ManualReset = false);
	SHARED_INLINE void CreateW(const wchar_t* pName, bool ManualReset = false);
	SHARED_INLINE void Set(void);
	SHARED_INLINE void Reset(void);
	SHARED_INLINE bool Wait(unsigned long msTimeout = INFINITE); //Returns true if timed out; use INFINITE to wait indefinitely
	SHARED_INLINE HANDLE GetHandle();
};

class CFunctionTime {
private:
	DWORD		m_Start_Time;
	const char*	m_pFunction;
public:
	SHARED_INLINE CFunctionTime(const char* pFunction);
	SHARED_INLINE ~CFunctionTime();
};

#define FUNC_TIME()	CFunctionTime _this_FuncTime_(__FUNCTION__)

template<typename T> class CAutoSetReset {
private:
	T*	m_pValue;
	T	m_Old;
public:
	CAutoSetReset(T* pVal, T New){
		m_Old = *pVal;
		m_pValue = pVal;
		*pVal = New;
	}
	~CAutoSetReset(){
		*m_pValue = m_Old;
	}
};


template<typename Counter> class _i_reference_target {
protected:
	Counter	m_RefCount;
public:
	_i_reference_target(){
		m_RefCount = 0;
	}
	virtual ~_i_reference_target(){
	}
	virtual void AddRef(){
		m_RefCount++;
	}
	virtual void Release(){
		if (--m_RefCount <= 0)
			delete this;
	}
	class CAutoRef {
	};
};

class CRefCounter {
protected:
	int	m_RefCount;
public:
	CRefCounter(){
		m_RefCount = 0;
	}
	virtual ~CRefCounter(){
	}
	virtual void AddRef(){
		m_RefCount++;
	}
	virtual void Release(){
		if (--m_RefCount <= 0)
			delete this;
	}
	class CAutoRef {
	protected:
		CRefCounter*	m_pRef;
		const char*		m_pName;
	public:
		CAutoRef(CRefCounter* pRef, bool add = true, const char* pName = NULL){
			m_pRef = pRef;
			if (m_pRef && add)
				m_pRef->AddRef();
			m_pName = pName;
		}
		~CAutoRef(){
			if (m_pRef)
				m_pRef->Release();
		}
	};
};

template<typename T> class RefPtr {
private:
	T*	m_pPtr;
public:
	RefPtr(){
		m_pPtr = NULL;
	}
	~RefPtr(){
		if (m_pPtr){
			m_pPtr->Release();
			m_pPtr = NULL;
		}
	}
	T* operator=(T* b){
		if (m_pPtr)
			m_pPtr->Release();
		m_pPtr = b;
		if (m_pPtr)
			m_pPtr->AddRef();
		return m_pPtr;
	}
	T* operator->(){
		return m_pPtr;
	}
};


//Enables more aggressive autoref output
//#define TYPE2_AREF

#ifdef TYPE2_AREF

class AutoRef {
private:
	IUnknown*	m_pInterface;
	const char*	m_pName;
	const char*	m_pFile;
	const char*	m_pFunction;
	int			m_Line;
	ULONG		m_EntryRefCount;
	int			m_ID;
	static int	s_ID;
public:
	AutoRef(IUnknown* pInterface, const char* pFile, int line, const char* pFunction, bool AddRef = true, const char* pName = NULL){
		m_ID = s_ID++;
		m_pInterface = pInterface;
		if (!pInterface)
			return;
		m_pName = pName;
		m_pFile = pFile;
		m_pFunction = pFunction;
		m_Line = line;

		m_EntryRefCount = m_pInterface->AddRef()-1;
		if (!AddRef){
			m_pInterface->Release();
			LogFormated(L" AutoRef   ID: %6d  Entry RefCount: %5u                   Name: %-20S  File: %S  Line: %d  Function: %S\n",
				m_ID, m_EntryRefCount, pName, pFile, line, pFunction);
		} else
			LogFormated(L" AutoRef   ID: %6d  Entry RefCount: %5u  RefCount: %5u  Name: %-20S  File: %S  Line: %d  Function: %S\n",
				m_ID, m_EntryRefCount, m_EntryRefCount+2, pName, pFile, line, pFunction);
	}
	~AutoRef(){
		ULONG RefCount;
		RefCount = 0;
		if (m_pInterface)
			RefCount = m_pInterface->Release();

		LogFormated(L"~AutoRef %C ID: %6d  Entry RefCount: %5u  RefCount: %5u  Name: %-20S  File: %S  Line: %d  Function: %S\n",
			RefCount == 0 ? L'*' : L' ', m_ID, m_EntryRefCount, RefCount, m_pName, m_pFile, m_Line, m_pFunction);
	}
};



#define AREF(_instanceptr, _addref) \
	AutoRef COMBINEDYN(__autoref_, __COUNTER__)(_instanceptr, __FILE__, __LINE__, __FUNCTION__, _addref, #_instanceptr);

#define AREF2(_instanceptr) \
	AutoRef COMBINEDYN(__autoref_, __COUNTER__)(_instanceptr, __FILE__, __LINE__, __FUNCTION__, true, #_instanceptr);

#define AREF3(_instanceptr) \
	AutoRef COMBINEDYN(__autoref_, __COUNTER__)(_instanceptr, __FILE__, __LINE__, __FUNCTION__, false, #_instanceptr);


#else //TYPE2_AREF

class AutoRef {
private:
	IUnknown*		m_pInterface;
	IConstUnknown*	m_pConstInterface;
	const char*		m_pName;
public:
	AutoRef(IUnknown* pInterface, bool AddRef = true, const char* pName = NULL){
		m_pInterface = pInterface;
		m_pConstInterface = NULL;
		if (!pInterface)
			return;
		m_pName = pName;

		if (AddRef)
			m_pInterface->AddRef();
	}
	AutoRef(IConstUnknown* pInterface, bool AddRef = true, const char* pName = NULL){
		m_pConstInterface = pInterface;
		m_pInterface = NULL;
		if (!m_pConstInterface)
			return;
		m_pName = pName;

		if (AddRef)
			m_pConstInterface->AddRef();
	}
	~AutoRef(){
		if (m_pInterface)
			m_pInterface->Release();
		if (m_pConstInterface)
			m_pConstInterface->Release();
	}
};

#define AREF(_instanceptr, _addref) \
	AutoRef COMBINEDYN(__autoref_, __COUNTER__)(_instanceptr, _addref, #_instanceptr);

#define AREF2(_instanceptr) \
	AutoRef COMBINEDYN(__autoref_, __COUNTER__)(_instanceptr, true, #_instanceptr);

#define AREF3(_instanceptr) \
	AutoRef COMBINEDYN(__autoref_, __COUNTER__)(_instanceptr, false, #_instanceptr);


#define CAREF(_instanceptr, _addref) \
	CRefCounter::CAutoRef COMBINEDYN(__CRefCounter_autoref_, __COUNTER__)(_instanceptr, _addref, #_instanceptr);

#define CAREF2(_instanceptr) \
	CRefCounter::CAutoRef COMBINEDYN(__CRefCounter_autoref_, __COUNTER__)(_instanceptr, true, #_instanceptr);

#define CAREF3(_instanceptr) \
	CRefCounter::CAutoRef COMBINEDYN(__CRefCounter_autoref_, __COUNTER__)(_instanceptr, false, #_instanceptr);

#endif //!TYPE2_AREF


#endif //_BASECLASSES_H_
