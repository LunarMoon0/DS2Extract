#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <Windows.h>
#include <stdio.h>
#include "Basetypes.h"
#include "String.h"
#include "StringBuffer.h"
#include "Lists.h"

//CDynMemoryBuffer


//Enables use of VirtualAlloc/VirtualFree instead of malloc/free, allowing execution on the data
//#define USE_VALLOC

class CDynMemoryBuffer {
private:
	size_32	m_Align;
	size_32	m_Size;
	size_32	m_AllocatedSize;
	void*			m_pBuffer;
	size_32	m_Offset;

public:
	CDynMemoryBuffer(size_32 align);
	~CDynMemoryBuffer();
	void Clear(bool freedata);
	bool EnsureSize(size_32 size);
	__inline bool Alloc(size_32 size);
	__inline size_32 GetAllocatedSize();
	__inline size_32 GetSize();
	__inline bool SetSize(size_32 size);
	__inline void* GetPtr();
	__inline LPBYTE GetBytePtr();
	bool Append(const void* pData, size_32 datasize);
	__inline operator void*();

	class CRef {
	protected:
		CDynMemoryBuffer*	m_pBuffer;
		unsigned int		m_Offset;
	public:
		CRef(CDynMemoryBuffer* pBuf, int alloc = 0);
		CRef(CDynMemoryBuffer& Buf, int alloc = 0);
		__inline void* GetPtr();
		__inline void* GetBytePtr();
		template <typename T> __inline T*Get();
	};

	template<typename T> class TRef : protected CRef {
	public:
		__inline TRef(CDynMemoryBuffer* pBuf, bool alloc = false);
		__inline TRef(CDynMemoryBuffer& Buf, bool alloc = false);
		__inline TRef(CDynMemoryBuffer* pBuf, int alloc = 0);
		__inline TRef(CDynMemoryBuffer& Buf, int alloc = 0) ;
		__inline T* operator()();
		__inline T* Get();
	};
	class CDynMemoryReader {
	private:
		CDynMemoryBuffer*	m_pBuffer;
		size_32				m_Offset;
		CStringBufferA		m_BufA;
		CStringBuffer		m_BufW;
	public:
		CDynMemoryReader(CDynMemoryBuffer* pBuffer){
			m_Offset = 0;
			m_pBuffer = pBuffer;
		}
		template<typename T> bool Get(T *p){
			if (m_pBuffer->GetSize() < m_Offset+sizeof(T))
				return false;
			memcpy(p, m_pBuffer->GetBytePtr()+m_Offset, sizeof(T));
			return m_Offset+= sizeof(T), true;
		}
#define READ_FUNC(_type, _name) \
	_type Get##_name(){ _type v; if (!Get(&v)) v = 0; return v; };
		READ_FUNC(INT8, Int8);
		READ_FUNC(INT16, Int16);
		READ_FUNC(INT32, Int32);
		READ_FUNC(INT64, Int64);

		READ_FUNC(UINT8, UInt8);
		READ_FUNC(UINT16, UInt16);
		READ_FUNC(UINT32, UInt32);
		READ_FUNC(UINT64, UInt64);

		READ_FUNC(float, Float);
		READ_FUNC(double, Double);

		bool GetRaw(void* buf, size_32 size){
			if (m_pBuffer->GetSize() < (size_32)(m_Offset+size))
				return false;
			memcpy(buf, m_pBuffer->GetBytePtr()+m_Offset, size);
			return m_Offset+= size, true;
		}
		bool SeekDelta(ssize_32 delta){
			if ((delta < 0 && (size_32)(-delta) > m_Offset)){
				return false;
			} else if (m_Offset+(size_32)delta > m_pBuffer->GetSize())
				return false;
			m_Offset += delta;
			return true;
		}
		bool SeekSet(size_32 offset){
			if (offset > m_pBuffer->GetSize())
				return false;
			m_Offset = offset;
			return true;
		}
		bool GetStringA(CStringA &str){
			size_32 len;
			if (!Get(&len))
				return false;
			m_BufA.Alloc(len);
			if (!GetRaw(m_BufA.GetBufferPtr(), (size_32)len))
				return false;
			str = m_BufA;
			return true;
		}
		bool GetStringW(CStringW &str){
			size_32 len;
			if (!Get(&len))
				return false;
			m_BufW.Alloc(len);
			if (!GetRaw(m_BufW.GetBufferPtr(), (size_32)len*sizeof(wchar_t)))
				return false;
			str = m_BufW;
			return true;
		}
		bool GetBuffer(CDynMemoryBuffer &buf){
			size_32 len, r, len2;
			buf.Clear(false);
			unsigned char lBuf[1024];
			CDynMemoryWriter w(&buf);
			if (!Get(&len))
				return false;
			buf.SetSize(0);
			if (!buf.EnsureSize(len))
				return false;
			len2 = len;
			for(;len;){
				r = min((size_32)len, sizeof(lBuf));
				if (!GetRaw(lBuf, r))
					return false;
				if (!w.PutRaw(lBuf, min((int)len, sizeof(lBuf))))
					return false;
				len -= r;
			}
			return buf.GetSize() == len2;
		}
	};

	
	class CDynMemoryWriter {
	private:
		CDynMemoryBuffer*	m_pBuffer;
		size_32				m_Offset;
	public:
		CDynMemoryWriter(CDynMemoryBuffer* pBuffer){
			m_pBuffer = pBuffer;
		}
		template<typename T> bool Put(const T *p){
			return m_pBuffer->Append(p, sizeof(T));
		}
#define WRITE_FUNC(_type, _name) \
	bool Put##_name(_type val){ return Put(&val); };
		WRITE_FUNC(INT8, Int8);
		WRITE_FUNC(INT16, Int16);
		WRITE_FUNC(INT32, Int32);
		WRITE_FUNC(INT64, Int64);

		WRITE_FUNC(UINT8, UInt8);
		WRITE_FUNC(UINT16, UInt16);
		WRITE_FUNC(UINT32, UInt32);
		WRITE_FUNC(UINT64, UInt64);

		WRITE_FUNC(float, Float);
		WRITE_FUNC(double, Double);

		bool PutRaw(const void* buf, size_32 size){
			if (!m_pBuffer->EnsureSize(m_pBuffer->GetSize()+size))
				return false;
			return m_pBuffer->Append(buf, size);
		}
		bool PutStringA(const char* str){
			size_32 len;
			if (!str){
				len = 0;
				return Put(&len);
			} else {
				len = strlen(str)+1;
				if (!Put(&len))
					return false;
				return PutRaw(str, (size_32)len);
			}
		}
		bool PutStringW(const wchar_t* str){
			size_32 len;
			if (!str){
				len = 0;
				return Put(&len);
			} else {
				len = wcslen(str)+1;
				if (!Put(&len))
					return false;
				return PutRaw(str, (size_32)len*sizeof(wchar_t));
			}
		}
		bool PutBuffer(CDynMemoryBuffer* buf){
			size_32 len;
			len = buf->GetSize();
			if (!Put(&len))
				return false;
			if (!m_pBuffer->EnsureSize(m_pBuffer->GetSize()+len))
				return false;
			return PutRaw(buf->GetPtr(), (size_32)len);
		}
		bool PutBuffer2(const void* mem, size_32 size){
			if (!Put(&size))
				return false;
			if (!m_pBuffer->EnsureSize(m_pBuffer->GetSize()+size))
				return false;
			return PutRaw(mem, (size_32)size);
		}
	};
};

//CDynMemoryBuffer inline impl

__inline bool CDynMemoryBuffer::Alloc(size_32 size){
	if (EnsureSize(size))
		return m_Size = size, true;
	return false;
}

__inline size_32 CDynMemoryBuffer::GetAllocatedSize(){
	return m_Size;
}

__inline size_32 CDynMemoryBuffer::GetSize(){
	return m_Size;
}

__inline bool CDynMemoryBuffer::SetSize(size_32 size){
	if (size <= m_AllocatedSize){
		m_Size = size;
		return true;
	}
	return false;
}

__inline void* CDynMemoryBuffer::GetPtr(){
	return m_pBuffer;
}
__inline LPBYTE CDynMemoryBuffer::GetBytePtr(){
	return (LPBYTE)m_pBuffer;
}

__inline CDynMemoryBuffer::operator void*(){
	return m_pBuffer;
}

//CDynMemoryBuffer::CRef impl

__inline CDynMemoryBuffer::CRef::CRef(CDynMemoryBuffer* pBuf, int alloc){
	m_pBuffer = pBuf;
	m_Offset = pBuf->GetSize();
	if (alloc)
		pBuf->Alloc(m_Offset+alloc);
}
__inline CDynMemoryBuffer::CRef::CRef(CDynMemoryBuffer& Buf, int alloc){
	m_pBuffer = &Buf;
	m_Offset = (&Buf)->GetSize();
	if (alloc)
		(&Buf)->Alloc(m_Offset+alloc);
}
__inline void* CDynMemoryBuffer::CRef::GetPtr(){
	return m_pBuffer->GetBytePtr()+m_Offset;
}
__inline void* CDynMemoryBuffer::CRef::GetBytePtr(){
	return m_pBuffer->GetBytePtr()+m_Offset;
}
template<typename T> __inline T*CDynMemoryBuffer::CRef::Get(){
	return (T*)(m_pBuffer->GetBytePtr()+m_Offset);
}

//CDynMemoryBuffer::TRef impl

template<typename T> __inline CDynMemoryBuffer::TRef<T>::TRef(CDynMemoryBuffer* pBuf, bool alloc) : CRef(pBuf) {
	if (alloc)
		m_pBuffer->Alloc(m_Offset+sizeof(T));
}
template<typename T> __inline CDynMemoryBuffer::TRef<T>::TRef(CDynMemoryBuffer& Buf, bool alloc) : CRef(Buf) {
	if (alloc)
		m_pBuffer->Alloc(m_Offset+sizeof(T));
}
template<typename T> __inline CDynMemoryBuffer::TRef<T>::TRef(CDynMemoryBuffer* pBuf, int alloc) : CRef(pBuf) {
	if (alloc)
		m_pBuffer->Alloc(m_Offset+((sizeof(T)*alloc)));
}
template<typename T> __inline CDynMemoryBuffer::TRef<T>::TRef(CDynMemoryBuffer& Buf, int alloc) : CRef(Buf) {
	if (alloc)
		m_pBuffer->Alloc(m_Offset+((sizeof(T)*alloc)));
}
template<typename T> __inline T* CDynMemoryBuffer::TRef<T>::operator()(){
	return (T*)(m_pBuffer->GetBytePtr()+m_Offset);
}
template<typename T> __inline T* CDynMemoryBuffer::TRef<T>::Get(){
	return (T*)(m_pBuffer->GetBytePtr()+m_Offset);
}

//TArray impl

template<typename T, int align> class TArray {
private:
	CDynMemoryBuffer	m_Buffer;
public:
	TArray();
	~TArray();
	void Reset(bool clear);
	T* Get(int index);
	T* Alloc();
	T &AllocRef();
	unsigned int GetCount();
	unsigned int GetRawSize();
	T* Array();
	T* AllocArray(int count);
};

template<typename T, int align> TArray<T, align>::TArray() : m_Buffer(sizeof(T)*align) {
}
template<typename T, int align> TArray<T, align>::~TArray(){
	m_Buffer.Clear(true);
}
template<typename T, int align> void TArray<T, align>::Reset(bool clear){
	m_Buffer.Clear(clear);
}
template<typename T, int align> T* TArray<T, align>::Get(int index){
	unsigned int count;
	if (index < 0)
		return NULL;
	count = m_Buffer.GetSize()/sizeof(T);
	if (i >= count)
		return NULL;
	return m_Buffer.GetBytePtr()+(i*sizeof(T));
}
template<typename T, int align> T* TArray<T, align>::Alloc(){
	unsigned int size;
	size = m_Buffer.GetSize();
	if (!m_Buffer.Alloc(size+sizeof(T)))
		return NULL;
	return (T*)(m_Buffer.GetBytePtr()+size);
}
template<typename T, int align> T &TArray<T, align>::AllocRef(){
	return (T&)Alloc();
}
template<typename T, int align> unsigned int TArray<T, align>::GetCount(){
	return m_Buffer.GetSize()/sizeof(T);
}
template<typename T, int align> unsigned int TArray<T, align>::GetRawSize(){
	return m_Buffer.GetSize();
}
template<typename T, int align> T* TArray<T, align>::Array(){
	return (T*)m_Buffer.GetPtr();
}
template<typename T, int align> T* TArray<T, align>::AllocArray(int count){
	if (!m_Buffer.Alloc(count*sizeof(T)))
		return NULL;
	return (T*)m_Buffer.GetPtr();
}

interface IMemoryBuffer : public CDynMemoryBuffer{
private:
	CDynMemoryReader	m_Reader;
	CDynMemoryWriter	m_Writer;
	size_32				m_RefCount;
	~IMemoryBuffer(){}
public:
	inline IMemoryBuffer(unsigned int align) : CDynMemoryBuffer(align), m_Reader(this), m_Writer(this){
		m_RefCount = 1;
	}
	inline size_32 AddRef(){
		return ++m_RefCount;
	}
	inline size_32 Release(){
		if (--m_RefCount == 0)
			return (delete this), 0;
		return m_RefCount;
	}
	CDynMemoryWriter* w(){
		return &m_Writer;
	}
	CDynMemoryReader* r(){
		return &m_Reader;
	}
	CDynMemoryBuffer* b(){
		return (CDynMemoryBuffer*)this;
	}
};

class CBufferManager {
private:
	static CriticalSectionClass	s_CS;
	static SList<IMemoryBuffer>	s_Available;
	static SList<IMemoryBuffer>	s_Used;
	enum {
		CBufferManager_Align = 2048,
	};
public:
	CBufferManager();
	~CBufferManager();
	static IMemoryBuffer* Get(size_32 sizehint);
	static void Return(IMemoryBuffer* pBuf);
	static void Initialize();
	static void Shutdown();
private:
	static CBufferManager	s_AutoDestroy;
};


#endif //!_BUFFER_H_
