#include "Buffer.h"


bool CDynMemoryBuffer::EnsureSize(size_32 size){
	size = (((size)/m_Align)+(size%m_Align ? 1 : 0))*m_Align;
	void *pBuf;
	if (size <= m_AllocatedSize)
		return true;
#ifdef USE_VALLOC
	pBuf = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#else //USE_VALLOC
	pBuf = g_pAllocator(size);
#endif //!USE_VALLOC
	if (m_pBuffer){
		if (pBuf)
			memcpy(pBuf, m_pBuffer, m_AllocatedSize);
#ifdef USE_VALLOC
		VirtualFree(m_pBuffer, 0, MEM_DECOMMIT);
#else //USE_VALLOC
		g_pFreeer(m_pBuffer);
#endif //!USE_VALLOC
	}
	if (!pBuf)
		return false;
	memset(((LPBYTE)pBuf)+m_AllocatedSize, 0, size-m_AllocatedSize);
	m_pBuffer = pBuf;
	m_AllocatedSize = size;
	return true;
}
CDynMemoryBuffer::CDynMemoryBuffer(size_32 align){
	m_Align = align;
	m_Size = m_AllocatedSize = m_Offset = 0;
	m_pBuffer = NULL;
}
CDynMemoryBuffer::~CDynMemoryBuffer(){
	Clear(true);
}
void CDynMemoryBuffer::Clear(bool freedata){
	if (freedata){
		if (m_pBuffer)
#ifdef USE_VALLOC
			VirtualFree(m_pBuffer, 0, MEM_DECOMMIT);
#else //USE_VALLOC
			g_pFreeer(m_pBuffer);
#endif //!USE_VALLOC
		m_pBuffer = NULL;
		m_AllocatedSize = 0;
	} else {
		memset(m_pBuffer, 0, m_AllocatedSize);
	}
	m_Size = 0;
}

bool CDynMemoryBuffer::Append(const void* pData, size_32 datasize){
	size_32 size;
	size = m_Size;
	if (!Alloc(size+datasize))
		return false;
	memcpy(GetBytePtr()+size, pData, datasize);
	return true;
}

/*template<typename T, int align> class TArray {
private:
	CDynMemoryBuffer	m_Buffer;
public:
	TArray() : m_Buffer(sizeof(T)*align) {
	}
	~TArray(){
		m_Buffer.Clear(true);
	}
	void Reset(bool clear){
		m_Buffer.Clear(clear);
	}
	T* Get(int index){
		unsigned int count;
		if (index < 0)
			return NULL;
		count = m_Buffer.GetSize()/sizeof(T);
		if (i >= count)
			return NULL;
		return m_Buffer.GetBytePtr()+(i*sizeof(T));
	}
	T* Alloc(){
		unsigned int size;
		size = m_Buffer.GetSize();
		if (!m_Buffer.Alloc(size+sizeof(T)))
			return NULL;
		return (T*)(m_Buffer.GetBytePtr()+size);
	}
	T &AllocRef(){
		return (T&)Alloc();
	}
	unsigned int GetCount(){
		return m_Buffer.GetSize()/sizeof(T);
	}
	unsigned int GetRawSize(){
		return m_Buffer.GetSize();
	}
	T* Array(){
		return (T*)m_Buffer.GetPtr();
	}
	T* AllocArray(int count){
		if (!m_Buffer.Alloc(count*sizeof(T)))
			return NULL;
		return (T*)m_Buffer.GetPtr();
	}
};*/

/*CStringArray::CStringArray() : m_Strings(2048){
}

CStringArray::~CStringArray(){
	Clear();
}

void CStringArray::Clear(){
	m_StringPtrs.Reset(true);
	m_Strings.Clear(true);
}*/


/*void* CStringArray::GetData(){
	return m_Strings.GetPtr();
}

unsigned int CStringArray::ReturnDataSize(){
	return m_Strings.GetSize();
}*/

CriticalSectionClass	CBufferManager::s_CS;
SList<IMemoryBuffer>	CBufferManager::s_Available;
SList<IMemoryBuffer>	CBufferManager::s_Used;

CBufferManager::CBufferManager(){
	Initialize();
}

CBufferManager::~CBufferManager(){
	Shutdown();
}

IMemoryBuffer* CBufferManager::Get(size_32 sizehint){
	GenericSLNode<IMemoryBuffer>* node;
	IMemoryBuffer* upper, *lower;
	size_32 uppersize, lowersize;
	size_32 size;
	s_CS.Enter();
	lower = upper = NULL;
	uppersize = 0xFFFFFFFF;
	lowersize = 0;
	for (node = s_Available.Get_Head(); node; node = node->m_pNext){
		size = node->m_pData->GetAllocatedSize();
		if (size < sizehint && size >= lowersize){
			lowersize = size;
			lower = node->m_pData;
		}
		if (size >= sizehint && size <= uppersize){
			uppersize = size;
			upper = node->m_pData;
		}
	}
	if (upper){
		s_Available.Remove(upper);
		s_Used.Add_Head(upper);
		upper->SetSize(0);
		upper->AddRef();
		return s_CS.Leave(), upper;
	}
	if (lower){
		s_Available.Remove(lower);
		s_Used.Add_Head(lower);
		lower->EnsureSize(sizehint);
		lower->SetSize(0);
		lower->AddRef();
		return s_CS.Leave(), lower;
	}
	upper = new IMemoryBuffer(CBufferManager_Align);
	if (upper){
		s_Used.Add_Tail(upper);
		upper->EnsureSize(sizehint);
		upper->AddRef();
		return upper;
	}
	return s_CS.Leave(), upper;
}

void CBufferManager::Return(IMemoryBuffer* pBuf){
	s_CS.Enter();
	if (s_Used.Is_In_List(pBuf)){
		s_Used.Remove(pBuf);
		s_Available.Add_Tail(pBuf);
		pBuf->Release();
	}
	s_CS.Leave();
}

void CBufferManager::Initialize(){
}

void CBufferManager::Shutdown(){
	GenericSLNode<IMemoryBuffer>* node;
	s_CS.Enter();
	for (node = s_Available.Get_Head(); node; node = node->m_pNext){
		node->m_pData->Release();
	}
	for (node = s_Used.Get_Head(); node; node = node->m_pNext){
		node->m_pData->Release();
	}
	s_Available.Remove_All();
	s_Used.Remove_All();
	s_CS.Leave();
}