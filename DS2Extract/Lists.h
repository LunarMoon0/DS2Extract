#ifndef _LISTS_H_
#define _LISTS_H_

#include "Memory.h"
#include "Hash.h"


#define HASHLIST_USE_OPERATORS
#define HASHLIST_USE_POOLS
#define HASH_LIST_ALIGN			(128)//*/

//#define SLIST_CS

/*
SList
*/

template <typename T> struct GenericSLNode : public AutoPoolClass<GenericSLNode<T>, 256> {
	GenericSLNode<T>*	m_pNext;	
	T*					m_pData;
};


template <typename T> class SList {
private:
#ifdef SLIST_CS
	CriticalSectionClass m_CS;
#endif //SLIST_CS
	GenericSLNode<T> *m_pHead;
	GenericSLNode<T> *m_pTail;
	
public:
	typedef void (SListRemovalCallback)(SList<T> *pList, T *pObject, void *pCookie);
	static void DefaultCallback(SList<T>* pList, T* pObject, void* pCookie);


	SList(void);
	~SList(void);
	GenericSLNode<T> *Get_Head(void);
	GenericSLNode<T> *Get_Tail(void);
	bool Add_Head(T *pObject);
	bool Add_Head(SList<T> &List);
	bool Add_Tail(T *pObject);
	bool Add_Tail(SList<T> &List);
	T *Remove_Head(void);
	T *Remove_Tail(void);
	bool Remove(T *pObject);
	void Remove_All(SListRemovalCallback *pCallback = NULL, void *pCookie = NULL); // remember: memleak if you don't have a callback to destroy any unused objects
	bool Insert_Before(T *pObject, T *pBefore);
	bool Insert_After(T *pObject, T *pAfter);
	bool Is_Empty(void);
	int Get_Count(void);
	bool Is_In_List(T *pObject);
	GenericSLNode<T>* operator [] (int idx);

};

SHARED_INLINE template <typename T> void SList<T>::DefaultCallback(SList<T>* pList, T* pObject, void* pCookie){
	delete pObject;
}

SHARED_INLINE template <typename T> SList<T>::SList(void) {
	m_pHead = m_pTail = NULL;
}

SHARED_INLINE template <typename T> SList<T>::~SList(void) {
	Remove_All(NULL, NULL);
}

SHARED_INLINE template <typename T> GenericSLNode<T> *SList<T>::Get_Head(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	return m_pHead;
}

SHARED_INLINE template <typename T> GenericSLNode<T> *SList<T>::Get_Tail(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	return m_pTail;
}

SHARED_INLINE template <typename T> bool SList<T>::Add_Head(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	GenericSLNode<T> *pTemp;
	if (!pObject) return false;
	pTemp = new GenericSLNode<T>;
	pTemp->m_pNext = m_pHead;
	pTemp->m_pData = pObject;
	m_pHead = pTemp;
	if (!m_pTail) m_pTail = pTemp;
	return true;
}

template <typename T> bool SList<T>::Add_Head(SList<T> &List) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	GenericSLNode<T> *pTemp = NULL, *pNext = NULL, *pCurrent = NULL;
	if(!List.m_pHead) return false;
	pNext = List.m_pHead;
	while (pNext){
		if (pCurrent){
			pTemp = new GenericSLNode;
			pTemp->m_pData = pNext->m_pData;
			pTemp->m_pNext = pCurrent->m_pNext;
			pCurrent->m_pNext = pTemp;
			pCurrent = pTemp;
		} else {
			this->Add_Head((T*)pNext->m_pData);
			pCurrent = m_pHead;
		}
		pNext = pNext->m_pNext;
	}
	return true;
}

SHARED_INLINE template <typename T> bool SList<T>::Add_Tail(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	GenericSLNode<T> *pTemp;
	if (!pObject) return false;
	pTemp = new GenericSLNode<T>;
	pTemp->m_pData = pObject;
	pTemp->m_pNext = NULL;
	if (!m_pHead)
		m_pHead = m_pTail = pTemp;
	else {
		m_pTail->m_pNext = pTemp;
		m_pTail = pTemp;
	}
	return true;
}

SHARED_INLINE template <typename T> bool SList<T>::Add_Tail(SList<T> &List) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	GenericSLNode<T> *pNext = List.m_pHead;
	if (!pNext) return false;
	while (pNext){
		Add_Tail((T*)pNext->m_pData);
		pNext = pNext->m_pNext;
	}
	return true;
}

SHARED_INLINE template <typename T> T *SList<T>::Remove_Head(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	GenericSLNode<T> *pTemp;
	T *pObject;
	if (!(pTemp = m_pHead)) return NULL;
	m_pHead = m_pHead->m_pNext;
	pObject = (T*)pTemp->m_pData;
	if (!pTemp->m_pNext)
		m_pTail = NULL;
	delete pTemp;
	return pObject;
}

SHARED_INLINE template <typename T> T *SList<T>::Remove_Tail(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pObject;
	if (!m_pHead) return NULL;
	pObject = (T*)m_pTail->m_pData;
	return Remove(pObject) ? pObject : NULL;
}

template <typename T> bool SList<T>::Remove(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	GenericSLNode<T> *pTemp, *pTemp2;
	if (!pObject || !(pTemp = m_pHead))
		return false;
	if (m_pHead->m_pData == pObject)
		return Remove_Head() ? true : false;

	while (pTemp && pTemp->m_pNext){
		if (pTemp->m_pNext->m_pData == pObject)
			break;
		pTemp = pTemp->m_pNext;
	}
	if (!pTemp->m_pNext || pTemp->m_pNext->m_pData != pObject)
		return false;
	pTemp2 = pTemp->m_pNext;
	pTemp->m_pNext = pTemp->m_pNext->m_pNext;
	if (m_pTail == pTemp2)
		m_pTail = pTemp;
	delete pTemp2;
	return true;

}

SHARED_INLINE template <typename T> void SList<T>::Remove_All(SListRemovalCallback *pCallback, void *pCookie) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	GenericSLNode<T> *pTemp, *pTemp2;
	if ((pTemp = m_pHead)){
		while (pTemp){
			pTemp2 = pTemp->m_pNext;
			if (pCallback)
				pCallback(this, pTemp->m_pData, pCookie);
			delete pTemp;
			pTemp = pTemp2;
		}
	}
	m_pHead = m_pTail = NULL;
}

template <typename T> bool SList<T>::Insert_Before(T *pObject, T *pBefore) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	GenericSLNode<T> *pTemp, *pNext, *pCurrent;
	if (!pObject) return false;
	if (!m_pHead || !pBefore)
		return Add_Head(pObject);

	pCurrent = m_pHead;
	while(true) {
		pNext = pCurrent->m_pNext;
		if (!pNext || pNext->m_pData == pBefore) break;
		pCurrent = pNext;
	}
	if (!pCurrent->m_pNext || pCurrent->m_pNext->m_pData != pBefore) return false;
	pTemp = new GenericSLNode<T>;
	pTemp->m_pData = pObject;
	pTemp->m_pNext = NULL;
	pTemp->m_pNext = pCurrent->m_pNext;
	pCurrent->m_pNext = pTemp;
	return true;

}

template <typename T> bool SList<T>::Insert_After(T *pObject, T *pAfter) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	GenericSLNode<T> *pTemp, *pCurrent;
	if (!pObject) return false;
	if (!pAfter || !m_pHead)
		return Add_Head(pObject);

	pCurrent = m_pHead;
	while (pCurrent){
		if (pCurrent->m_pData == pAfter) break;
		pCurrent = pCurrent->m_pNext;
	}
	if (!pCurrent || pCurrent->m_pData != pAfter) return false;
	if (pCurrent == m_pTail)
		return Add_Tail(pObject);
	pTemp = new GenericSLNode<T>;
	pTemp->m_pData = pObject;
	pTemp->m_pNext = pCurrent->m_pNext;
	pCurrent->m_pNext = pTemp;
	return true;

}

SHARED_INLINE template <typename T> bool SList<T>::Is_Empty(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	return m_pHead ? false : true;
}

SHARED_INLINE template <typename T> int SList<T>::Get_Count(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	int count = 0;
	GenericSLNode<T> *pTemp;
	if ((pTemp = m_pHead)){
		while (pTemp){
			count++;
			pTemp = pTemp->m_pNext;
		}
	}
	return count;
}


SHARED_INLINE template <typename T> bool SList<T>::Is_In_List(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	int count = 0;
	GenericSLNode<T> *pTemp;
	pTemp = m_pHead;
	while (pTemp){
		if (pTemp->m_pData == pObject)
			return true;
		pTemp = pTemp->m_pNext;
	}

	return false;
}

SHARED_INLINE template <typename T> GenericSLNode<T>* SList<T>::operator[] (int idx) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	int i;
	GenericSLNode<T> *pNode;
	
	pNode = m_pHead;

	i = 0;
	while (pNode){
		if (i == idx)
			return pNode;
		pNode = pNode->m_pNext;
	}
	return NULL;
}

/*
SList2
*/

template <typename T> class SList2 {
private:
#ifdef SLIST_CS
	CriticalSectionClass	m_CS;
#endif //SLIST_CS
	T*						m_pHead;
	T*						m_pTail;
public:
	typedef void (SList2RemovalCallback)(SList2<T> *pList, T *pObject, void *pCookie);

	SList2(void);
	~SList2(void);
	T *Get_Head(void);
	T *Get_Tail(void);
	bool Add_Head(T *pObject);
	bool Add_Head(SList2<T> &List);
	bool Add_Tail(T *pObject);
	bool Add_Tail(SList2<T> &List);
	T *Remove_Head(void);
	T *Remove_Tail(void);
	bool Remove(T *pObject);
	void Remove_All(SList2RemovalCallback *pCallback = NULL, void *pCookie = NULL); // remember: memleak if you don't have a callback to destroy any unused objects
	bool Insert_Before(T *pObject, T *pBefore);
	bool Insert_After(T *pObject, T *pAfter);
	bool Is_Empty(void);
	int Get_Count(void);
	bool Is_In_List(T *pObject);
	T* operator [] (int idx);
};


SHARED_INLINE template <typename T> SList2<T>::SList2(void) {
	m_pHead = m_pTail = NULL;
}

SHARED_INLINE template <typename T> SList2<T>::~SList2(void) {
	Remove_All(NULL, NULL);
}

SHARED_INLINE template <typename T> T*SList2<T>::Get_Head(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	return m_pHead;
}

SHARED_INLINE template <typename T> T *SList2<T>::Get_Tail(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	return m_pTail;
}

SHARED_INLINE template <typename T> bool SList2<T>::Add_Head(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	if (!pObject) return false;
	pObject->m_pNext = m_pHead;
	m_pHead = pObject;
	if (!m_pTail) m_pTail = pObject;
	return true;
}

template <typename T> bool SList2<T>::Add_Head(SList2<T> &List) {
	/*CriticalSectionClass::LockClass p_Lock(&this->m_CS);
	GenericSLNode<T> *pTemp = NULL, *pNext = NULL, *pCurrent = NULL;
	if(!List.m_pHead) return false;
	pNext = List.m_pHead;
	while (pNext){
		if (pCurrent){
			pTemp = new GenericSLNode;
			pTemp->m_pData = pNext->m_pData;
			pTemp->m_pNext = pCurrent->m_pNext;
			pCurrent->m_pNext = pTemp;
			pCurrent = pTemp;
		} else {
			this->Add_Head((T*)pNext->m_pData);
			pCurrent = m_pHead;
		}
		pNext = pNext->m_pNext;
	}//*/
	return true;
}

SHARED_INLINE template <typename T> bool SList2<T>::Add_Tail(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	if (!pObject) return false;
	pObject->m_pNext = NULL;
	if (!m_pHead)
		m_pHead = m_pTail = pObject;
	else {
		m_pTail->m_pNext = pObject;
		m_pTail = pObject;
	}
	return true;
}

SHARED_INLINE template <typename T> bool SList2<T>::Add_Tail(SList2<T> &List) {
	/*CriticalSectionClass::LockClass p_Lock(&this->m_CS);
	GenericSLNode<T> *pNext = List.m_pHead;
	if (!pNext) return false;
	while (pNext){
		Add_Tail((T*)pNext->m_pData);
		pNext = pNext->m_pNext;
	}
	return true;//*/
}

SHARED_INLINE template <typename T> T *SList2<T>::Remove_Head(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pObject;
	if (!(pObject = m_pHead)) return NULL;
	m_pHead = pObject->m_pNext;
	return pObject;
}

SHARED_INLINE template <typename T> T *SList2<T>::Remove_Tail(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pObject;
	if (!m_pHead) return NULL;
	return Remove(m_pTail) ? pObject : NULL;
}

template <typename T> bool SList2<T>::Remove(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pTemp, *pTemp2;
	if (!pObject || !(pTemp = m_pHead))
		return false;
	if (m_pHead == pObject)
		return Remove_Head() ? true : false;

	while (pTemp && pTemp->m_pNext){
		if (pTemp->m_pNext == pObject)
			break;
		pTemp = pTemp->m_pNext;
	}
	if (!pTemp || !pTemp->m_pNext || pTemp->m_pNext != pObject)
		return false;
	pTemp2 = pTemp->m_pNext;
	pTemp->m_pNext = pTemp->m_pNext->m_pNext;
	if (m_pTail == pTemp2)
		m_pTail = pTemp;
	return true;

}

SHARED_INLINE template <typename T> void SList2<T>::Remove_All(SList2RemovalCallback *pCallback, void *pCookie) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pTemp, *pTemp2;
	if ((pTemp = m_pHead)){
		while (pTemp){
			pTemp2 = pTemp->m_pNext;
			if (pCallback)
				pCallback(this, pTemp, pCookie);
			pTemp = pTemp2;
		}
	}
	m_pHead = m_pTail = NULL;
}

template <typename T> bool SList2<T>::Insert_Before(T *pObject, T *pBefore) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pTemp, *pNext, *pCurrent;
	if (!pObject) return false;
	if (!m_pHead || !pBefore)
		return Add_Head(pObject);

	pCurrent = m_pHead;
	while(true) {
		pNext = pCurrent->m_pNext;
		if (!pNext || pNext == pBefore) break;
		pCurrent = pNext;
	}
	if (!pCurrent->m_pNext || pCurrent->m_pNext != pBefore) return false;
	pObject->m_pNext = pCurrent->m_pNext;
	pCurrent->m_pNext = pObject;
	return true;

}

template <typename T> bool SList2<T>::Insert_After(T *pObject, T *pAfter) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pTemp, *pCurrent;
	if (!pObject) return false;
	if (!pAfter || !m_pHead)
		return Add_Head(pObject);

	pCurrent = m_pHead;
	while (pCurrent){
		if (pCurrent == pAfter) break;
		pCurrent = pCurrent->m_pNext;
	}
	if (!pCurrent || pCurrent != pAfter) return false;
	if (pCurrent == m_pTail)
		return Add_Tail(pObject);
	pObject->m_pNext = pCurrent->m_pNext;
	pCurrent->m_pNext = pObject;
	return true;

}

SHARED_INLINE template <typename T> bool SList2<T>::Is_Empty(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	return m_pHead ? false : true;
}

SHARED_INLINE template <typename T> int SList2<T>::Get_Count(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	int count = 0;
	T *pTemp;
	if ((pTemp = m_pHead)){
		while (pTemp){
			count++;
			pTemp = pTemp->m_pNext;
		}
	}
	return count;
}


SHARED_INLINE template <typename T> bool SList2<T>::Is_In_List(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	int count = 0;
	T *pTemp;
	pTemp = m_pHead;
	while (pTemp){
		if (pTemp == pObject)
			return true;
		pTemp = pTemp->m_pNext;
	}

	return false;
}

SHARED_INLINE template <typename T> T* SList2<T>::operator[] (int idx) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	int i;
	T *pNode;
	
	pNode = m_pHead;

	i = 0;
	while (pNode){
		if (i == idx)
			return pNode;
		pNode = pNode->m_pNext;
	}
	return NULL;
}
/*
SList3
*/

template <typename T> class SList3 {
private:
#ifdef SLIST_CS
	CriticalSectionClass	m_CS;
#endif //SLIST_CS
	T*						m_pHead;
	T*						m_pTail;
public:
	typedef void (SList3RemovalCallback)(SList3<T> *pList, T *pObject, void *pCookie);

	SList3(void);
	~SList3(void);
	T *Get_Head(void);
	T *Get_Tail(void);
	bool Add_Head(T *pObject);
	bool Add_Head(SList3<T> &List);
	bool Add_Tail(T *pObject);
	bool Add_Tail(SList3<T> &List);
	T *Remove_Head(void);
	T *Remove_Tail(void);
	bool Remove(T *pObject);
	void Remove_All(SList3RemovalCallback *pCallback = NULL, void *pCookie = NULL); // remember: memleak if you don't have a callback to destroy any unused objects
	bool Insert_Before(T *pObject, T *pBefore);
	bool Insert_After(T *pObject, T *pAfter);
	bool Is_Empty(void);
	int Get_Count(void);
	bool Is_In_List(T *pObject);
	T* operator [] (int idx);
};


SHARED_INLINE template <typename T> SList3<T>::SList3(void) {
	m_pHead = m_pTail = NULL;
}

SHARED_INLINE template <typename T> SList3<T>::~SList3(void) {
	Remove_All(NULL, NULL);
}

SHARED_INLINE template <typename T> T*SList3<T>::Get_Head(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	return m_pHead;
}

SHARED_INLINE template <typename T> T *SList3<T>::Get_Tail(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	return m_pTail;
}

SHARED_INLINE template <typename T> bool SList3<T>::Add_Head(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	if (!pObject) return false;
	pObject->m_pNext2 = m_pHead;
	m_pHead = pObject;
	if (!m_pTail) m_pTail = pObject;
	return true;
}

template <typename T> bool SList3<T>::Add_Head(SList3<T> &List) {
	/*CriticalSectionClass::LockClass p_Lock(&this->m_CS);
	GenericSLNode<T> *pTemp = NULL, *pNext = NULL, *pCurrent = NULL;
	if(!List.m_pHead) return false;
	pNext = List.m_pHead;
	while (pNext){
		if (pCurrent){
			pTemp = new GenericSLNode;
			pTemp->m_pData = pNext->m_pData;
			pTemp->m_pNext2 = pCurrent->m_pNext2;
			pCurrent->m_pNext2 = pTemp;
			pCurrent = pTemp;
		} else {
			this->Add_Head((T*)pNext->m_pData);
			pCurrent = m_pHead;
		}
		pNext = pNext->m_pNext2;
	}//*/
	return true;
}

SHARED_INLINE template <typename T> bool SList3<T>::Add_Tail(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	if (!pObject) return false;
	pObject->m_pNext2 = NULL;
	if (!m_pHead)
		m_pHead = m_pTail = pObject;
	else {
		m_pTail->m_pNext2 = pObject;
		m_pTail = pObject;
	}
	return true;
}

SHARED_INLINE template <typename T> bool SList3<T>::Add_Tail(SList3<T> &List) {
	/*CriticalSectionClass::LockClass p_Lock(&this->m_CS);
	GenericSLNode<T> *pNext = List.m_pHead;
	if (!pNext) return false;
	while (pNext){
		Add_Tail((T*)pNext->m_pData);
		pNext = pNext->m_pNext2;
	}
	return true;//*/
}

SHARED_INLINE template <typename T> T *SList3<T>::Remove_Head(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pObject;
	if (!(pObject = m_pHead)) return NULL;
	m_pHead = pObject->m_pNext2;
	return pObject;
}

SHARED_INLINE template <typename T> T *SList3<T>::Remove_Tail(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pObject;
	if (!m_pHead) return NULL;
	return Remove(m_pTail) ? pObject : NULL;
}

template <typename T> bool SList3<T>::Remove(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pTemp, *pTemp2;
	if (!pObject || !(pTemp = m_pHead))
		return false;
	if (m_pHead == pObject)
		return Remove_Head() ? true : false;

	while (pTemp && pTemp->m_pNext2){
		if (pTemp->m_pNext2 == pObject)
			break;
		pTemp = pTemp->m_pNext2;
	}
	if (!pTemp->m_pNext2 || pTemp->m_pNext2 != pObject)
		return false;
	pTemp2 = pTemp->m_pNext2;
	pTemp->m_pNext2 = pTemp->m_pNext2->m_pNext2;
	if (m_pTail == pTemp2)
		m_pTail = pTemp;
	return true;

}

SHARED_INLINE template <typename T> void SList3<T>::Remove_All(SList3RemovalCallback *pCallback, void *pCookie) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pTemp, *pTemp2;
	if ((pTemp = m_pHead)){
		while (pTemp){
			pTemp2 = pTemp->m_pNext2;
			if (pCallback)
				pCallback(this, pTemp, pCookie);
			pTemp = pTemp2;
		}
	}
	m_pHead = m_pTail = NULL;
}

template <typename T> bool SList3<T>::Insert_Before(T *pObject, T *pBefore) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pTemp, *pNext, *pCurrent;
	if (!pObject) return false;
	if (!m_pHead || !pBefore)
		return Add_Head(pObject);

	pCurrent = m_pHead;
	while(true) {
		pNext = pCurrent->m_pNext2;
		if (!pNext || pNext == pBefore) break;
		pCurrent = pNext;
	}
	if (!pCurrent->m_pNext2 || pCurrent->m_pNext2 != pBefore) return false;
	pObject->m_pNext2 = pCurrent->m_pNext2;
	pCurrent->m_pNext2 = pObject;
	return true;

}

template <typename T> bool SList3<T>::Insert_After(T *pObject, T *pAfter) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	T *pTemp, *pCurrent;
	if (!pObject) return false;
	if (!pAfter || !m_pHead)
		return Add_Head(pObject);

	pCurrent = m_pHead;
	while (pCurrent){
		if (pCurrent == pAfter) break;
		pCurrent = pCurrent->m_pNext2;
	}
	if (!pCurrent || pCurrent != pAfter) return false;
	if (pCurrent == m_pTail)
		return Add_Tail(pObject);
	pObject->m_pNext2 = pCurrent->m_pNext2;
	pCurrent->m_pNext2 = pObject;
	return true;

}

SHARED_INLINE template <typename T> bool SList3<T>::Is_Empty(void) {
	return m_pHead ? false : true;
}

SHARED_INLINE template <typename T> int SList3<T>::Get_Count(void) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	int count = 0;
	T *pTemp;
	if ((pTemp = m_pHead)){
		while (pTemp){
			count++;
			pTemp = pTemp->m_pNext2;
		}
	}
	return count;
}


SHARED_INLINE template <typename T> bool SList3<T>::Is_In_List(T *pObject) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	int count = 0;
	GenericSLNode<T> *pTemp;
	pTemp = m_pHead;
	while (pTemp){
		if (pTemp == pObject)
			return true;
		pTemp = pTemp->m_pNext2;
	}

	return false;
}

SHARED_INLINE template <typename T> T* SList3<T>::operator[] (int idx) {
#ifdef SLIST_CS
	CriticalSectionClass::LockClass p_Lock(&this->m_CS);
#endif //SLIST_CS
	int i;
	T *pNode;
	
	pNode = m_pHead;

	i = 0;
	while (pNode){
		if (i == idx)
			return pNode;
		pNode = pNode->m_pNext2;
	}
	return NULL;
}

#include "String.h"

/*
HashList
*/

template <typename T, typename HashType, typename Key> struct HashNode 
#ifdef HASHLIST_USE_POOLS
: AutoPoolClass<HashNode<T, HashType, Key>, HASH_LIST_ALIGN>
#endif //HASHLIST_USE_POOLS
{
	T						m_Data;
	Key						m_Key;
	HashType				m_Hashkey;
	HashNode<T, HashType, Key>*	m_pNext;
};

template <typename T, typename HashType, typename Key> class HashList {
protected:
	int								m_Item_Count;
	HashNode<T, HashType, Key>**	m_pTable;
	int								m_Table_Count;

public:
	typedef typename HashNode<T, HashType, Key> Node;
	HashList(int Table_Count);
	~HashList();
	void FreeData(bool DeleteAll);
	bool Insert(HashType Hashkey, Node** ppNode, T Item, Key key);
	bool Delete(HashType Hashkey, T Item, Key key);
	bool GetFirst(HashType *pHashkey, T *Data);
	Node *Search(HashType Hashkey);
	Node *SearchContinue(Node *pPrevious, HashType Hashkey);
	int GetCount(void);
	int GetTableCount(void);
	Node *GetEntryNode(int table);

	Node *operator () (HashType HashKey);
	Node *operator () (Node *pPrevious, HashType HashKey);

	Node *operator () (const char *pString);
	Node *operator () (Node *pPrevious, const char *pString);

	Node *operator () (const wchar_t *pString);
	Node *operator () (Node *pPrevious, const wchar_t *pString);

	Node *Find(HashType hash, Key key);
};

template <typename T, typename HashType, typename Key> HashList<T, HashType, Key>::HashList(int Table_Count) {
	m_Table_Count = Table_Count;
#ifdef HASHLIST_USE_OPERATORS
	m_pTable = new Node*[Table_Count];
#else //HASHLIST_USE_OPERATORS
	m_pTable = (HashNode<T, HashType, Key>**)g_pAllocator(sizeof(HashNode<T, HashType, Key>*)*Table_Count);
#endif //HASHLIST_USE_OPERATORS
	Node *pNode;

	for (int i = 0; i < Table_Count; i++){
#ifdef HASHLIST_USE_OPERATORS
		pNode = new HashNode<T, HashType, Key>;
#else //HASHLIST_USE_OPERATORS
		pNode = (HashNode<T, HashType, Key>*)g_pAllocator(sizeof(HashNode<T, HashType, Key>));
#endif //HASHLIST_USE_OPERATORS
		//pNode->m_Data; = NULL;
		pNode->m_Hashkey = 0;
		pNode->m_pNext = NULL;
		m_pTable[i] = pNode;
	}
	m_Item_Count = 0;
}

template <typename T, typename HashType, typename Key> HashList<T, HashType, Key>::~HashList(void) {
	Node *pNode, *pTemp;
	for (int i = 0; i < m_Table_Count; i++){
		pNode = m_pTable[i]->m_pNext;
		while (pNode){
			pTemp = pNode->m_pNext;
#ifdef HASHLIST_USE_OPERATORS
			delete pNode;
#else //HASHLIST_USE_OPERATORS
			g_pFreeer(pNode);
#endif //HASHLIST_USE_OPERATORS
			pNode = pTemp;
		}
#ifdef HASHLIST_USE_OPERATORS
		delete m_pTable[i];
#else //HASHLIST_USE_OPERATORS
		g_pFreeer(m_pTable[i]);
#endif //HASHLIST_USE_OPERATORS
	}
#ifdef HASHLIST_USE_OPERATORS
	delete [] m_pTable;
#else //HASHLIST_USE_OPERATORS
	g_pFreeer(m_pTable);
#endif //HASHLIST_USE_OPERATORS
}

template <typename T, typename HashType, typename Key> void HashList<T, HashType, Key>::FreeData(bool DeleteAll) {
	Node *pNode, *pTemp;
	for (int i = 0; i < m_Table_Count; i++){
		pNode = m_pTable[i]->m_pNext;
		while (pNode){
			pTemp = pNode->m_pNext;
#ifdef HASHLIST_USE_OPERATORS
			delete pNode;
#else HASHLIST_USE_OPERATORS
			g_pFreeer(pNode);
#endif //HASHLIST_USE_OPERATORS
			pNode = pTemp;
		}
		if (DeleteAll){
#ifdef HASHLIST_USE_OPERATORS
			delete m_pTable[i];
#else //HASHLIST_USE_OPERATORS
			g_pFreeer(m_pTable[i]);
#endif //HASHLIST_USE_OPERATORS
			m_pTable[i] = NULL;
		} else
			m_pTable[i]->m_pNext = NULL;
	}
	if (DeleteAll){
#ifdef HASHLIST_USE_OPERATORS
		delete [] m_pTable;
#else //HASHLIST_USE_OPERATORS
		m_pTable = NULL;
#endif //HASHLIST_USE_OPERATORS
	}
	m_Item_Count = 0;
}

template <typename T, typename HashType, typename Key> bool HashList<T, HashType, Key>::Insert(HashType Hashkey, Node** ppNode, T Item, Key key) {
	Node *pNode;
	int table = (Hashkey & 0xFFFF) % m_Table_Count;
#ifdef HASHLIST_USE_OPERATORS
	pNode = new Node;
#else //HASHLIST_USE_OPERATORS
	pNode = (HashNode<T, HashType, Key>*)g_pAllocator(sizeof(HashNode<T, HashType, Key>));
#endif //HASHLIST_USE_OPERATORS
	if (!pNode)	return false;
	pNode->m_Hashkey = Hashkey;
	pNode->m_Data = Item;
	pNode->m_pNext = m_pTable[table]->m_pNext;
	pNode->m_Key = key;
	if (ppNode) *ppNode = pNode;
	m_pTable[table]->m_pNext = pNode;
	m_Item_Count++;
	return true;
}

template <typename T, typename HashType, typename Key> bool HashList<T, HashType, Key>::Delete(HashType Hashkey, T Item, Key key) {
	Node *pNode, *pTemp;
	int table = (Hashkey & 0xFFFF) % m_Table_Count;
	pNode = m_pTable[table];
	pTemp = pNode->m_pNext;
	while (pTemp){
		if (pTemp->m_Hashkey == Hashkey && pTemp->m_Key == key){
			pNode->m_pNext = pTemp->m_pNext;
#ifdef HASHLIST_USE_OPERATORS
			delete pTemp;
#else //HASHLIST_USE_OPERATORS
			g_pFreeer(pTemp);
#endif //HASHLIST_USE_OPERATORS
			return true;
		}
		pNode = pTemp;
		pTemp = pTemp->m_pNext;
	}
	return false;
}

SHARED_INLINE template <typename T, typename HashType, typename Key> bool HashList<T, HashType, Key>::GetFirst(HashType *pHashkey, T *pData) {
	Node *pNode;
	for (int i = 0; i < m_Table_Count; i++){
		if ((pNode = m_pTable[i]->m_pNext)){
			*pData = pNode->m_Data;
			*pHashkey = pNode->m_Hashkey;
			return true;
		}
	}
	return false;
}

SHARED_INLINE template <typename T, typename HashType, typename Key> typename HashList<T, HashType, Key>::Node *HashList<T, HashType, Key>::Search(HashType Hashkey) {
	Node *pNode;
	pNode = m_pTable[(Hashkey & 0xFFFF) % m_Table_Count]->m_pNext;
	while (pNode){
		if (pNode->m_Hashkey == Hashkey) return pNode;
		pNode = pNode->m_pNext;
	}
	return NULL;
}

SHARED_INLINE template <typename T, typename HashType, typename Key> typename HashList<T, HashType, Key>::Node *HashList<T, HashType, Key>::SearchContinue(typename HashList<T, HashType, Key>::Node *pPrevious, HashType Hashkey) {
	Node *pNode;
	if (!pPrevious) return NULL;
	pNode = pPrevious->m_pNext;
	while (pNode){
		if (pNode->m_Hashkey == Hashkey) return pNode;
		pNode = pNode->m_pNext;
	}
	return NULL;
}

SHARED_INLINE template <typename T, typename HashType, typename Key> int HashList<T, HashType, Key>::GetCount(void) {
	return m_Item_Count;
}

SHARED_INLINE template <typename T, typename HashType, typename Key> int HashList<T, HashType, Key>::GetTableCount(void) {
	return m_Table_Count;
}

SHARED_INLINE template <typename T, typename HashType, typename Key> typename HashList<T, HashType, Key>::Node *HashList<T, HashType, Key>::GetEntryNode(int table) {
	return m_pTable[table]->m_pNext;
}


SHARED_INLINE template <typename T, typename HashType, typename Key> typename HashList<T, HashType, Key>::Node *HashList<T, HashType, Key>::operator () (HashType HashKey) {
	return Search(HashKey);
}

SHARED_INLINE template <typename T, typename HashType, typename Key> typename HashList<T, HashType, Key>::Node *HashList<T, HashType, Key>::operator () (Node *pPrevious, HashType HashKey) {
	return SearchContinue(pPrevious, HashKey);
}

SHARED_INLINE template <typename T, typename HashType, typename Key> typename HashList<T, HashType, Key>::Node *HashList<T, HashType, Key>::operator () (const char *pString) {
	return Search((HashType)Get_Str_Hash(pString, false));
}

SHARED_INLINE template <typename T, typename HashType, typename Key> typename HashList<T, HashType, Key>::Node *HashList<T, HashType, Key>::operator () (Node *pPrevious, const char *pString) {
	return SearchContinue(pPrevious, (HashType)Get_Str_Hash(pString, false));
}

SHARED_INLINE template <typename T, typename HashType, typename Key> typename HashList<T, HashType, Key>::Node *HashList<T, HashType, Key>::operator () (const wchar_t *pString) {
	return Search((HashType)Get_Str_Hash(pString, false));
}

SHARED_INLINE template <typename T, typename HashType, typename Key> typename HashList<T, HashType, Key>::Node *HashList<T, HashType, Key>::operator () (Node *pPrevious, const wchar_t *pString){
	return SearchContinue(pPrevious, (HashType)Get_Str_HashW(pString, false));
}

SHARED_INLINE template <typename T, typename HashType, typename Key> typename HashList<T, HashType, Key>::Node *HashList<T, HashType, Key>::Find(HashType hash, Key key){
	Node *pNode;
	for (pNode = (*this)(hash); pNode; pNode = (*this)(pNode, hash)){
		if (pNode->m_Key == key)
			return pNode;
	}
	return NULL;
}

/*
ExternNodeHashList
*/

template <typename T, typename HashType> struct ExternNodeHashNode{
	T									m_Data;
	HashType							m_Hashkey;
	ExternNodeHashNode<T, HashType>*	m_pNext;
};

template <typename T, typename HashType> class ExternNodeHashList {
protected:
	int						m_Item_Count;
	ExternNodeHashNode<T, HashType>**	m_pTable;
	int						m_Table_Count;

public:
	ExternNodeHashList(int Table_Count);
	~ExternNodeHashList();
	void FreeData(bool DeleteAll);
	bool Insert(HashType Hashkey, ExternNodeHashNode<T, HashType>* pNode, T Item);
	bool Delete(HashType Hashkey, T Item);
	bool GetFirst(HashType *pHashkey, T *Data);
	ExternNodeHashNode<T, HashType> *Search(HashType Hashkey);
	ExternNodeHashNode<T, HashType> *SearchContinue(ExternNodeHashNode<T, HashType> *pPrevious, HashType Hashkey);
	int GetCount(void);
	int GetTableCount(void);
	ExternNodeHashNode<T, HashType> *GetEntryNode(int table);

	ExternNodeHashNode<T, HashType> *operator () (HashType HashKey);
	ExternNodeHashNode<T, HashType> *operator () (ExternNodeHashNode<T, HashType> *pPrevious, HashType HashKey);

	ExternNodeHashNode<T, HashType> *operator () (const char *pString);
	ExternNodeHashNode<T, HashType> *operator () (ExternNodeHashNode<T, HashType> *pPrevious, const char *pString);

	ExternNodeHashNode<T, HashType> *operator () (const wchar_t *pString);
	ExternNodeHashNode<T, HashType> *operator () (ExternNodeHashNode<T, HashType> *pPrevious, const wchar_t *pString);
};

template <typename T, typename HashType> ExternNodeHashList<T, HashType>::ExternNodeHashList(int Table_Count) {
	m_Table_Count = Table_Count;
	m_pTable = new ExternNodeHashNode<T, HashType>*[Table_Count];
	//ExternNodeHashNode<T, HashType> *pNode;

	memset(m_pTable, 0, sizeof(ExternNodeHashNode<T, HashType>*)*Table_Count);
	/*for (int i = 0; i < Table_Count; i++){
		pNode = new ExternNodeHashNode<T, HashType>;
		pNode->m_Data = NULL;
		pNode->m_Hashkey = 0;
		pNode->m_pNext = NULL;
		m_pTable[i] = pNode;
	}//*/
	m_Item_Count = 0;
}

template <typename T, typename HashType> ExternNodeHashList<T, HashType>::~ExternNodeHashList(void) {
	int i;
	ExternNodeHashNode<T, HashType> *pNode, *pTemp;

	for (i = 0; i < m_Table_Count; i++){
		pNode = m_pTable[i];
		while (pNode){
			pTemp = pNode->m_pNext;
			pNode->m_pNext = NULL;
			pNode = pTemp;
		}//*/
	}
	delete [] m_pTable;
}

template <typename T, typename HashType> void ExternNodeHashList<T, HashType>::FreeData(bool DeleteAll) {
	ExternNodeHashNode<T, HashType> *pNode, *pTemp;
	for (int i = 0; i < m_Table_Count; i++){
		pNode = m_pTable[i];
		while (pNode){
			pTemp = pNode->m_pNext;
			pNode->m_pNext = NULL;
			pNode = pTemp;
		}//*/
		if (!DeleteAll)
			m_pTable[i] = NULL;
	}
	if (DeleteAll){
		delete [] m_pTable;
		m_pTable = NULL;
	}
	m_Item_Count = 0;
}

template <typename T, typename HashType> bool ExternNodeHashList<T, HashType>::Insert(HashType Hashkey, ExternNodeHashNode<T, HashType>* pNode, T Item) {
	int table = (Hashkey & 0xFFFF) % m_Table_Count;
	if (!pNode)	return false;
	pNode->m_Hashkey = Hashkey;
	pNode->m_Data = Item;
	pNode->m_pNext = m_pTable[table];
	m_pTable[table] = pNode;
	m_Item_Count++;
	return true;
}

template <typename T, typename HashType> bool ExternNodeHashList<T, HashType>::Delete(HashType Hashkey, T Item) {
	ExternNodeHashNode<T, HashType> *pNode, *pTemp;
	int table = (Hashkey & 0xFFFF) % m_Table_Count;
	pTemp = pNode = m_pTable[table];

	if (!pTemp) return false;

	if (pTemp->m_Hashkey == Hashkey && pTemp->m_Data == Item){
		m_pTable[table] = NULL;
		return true;
	}

	pTemp = pTemp->m_pNext;

	while (pTemp){
		if (pTemp->m_Hashkey == Hashkey && pTemp->m_Data == Item){
			pNode->m_pNext = pTemp->m_pNext;
			return true;
		}
		pNode = pTemp;
		pTemp = pTemp->m_pNext;
	}
	return false;
}

SHARED_INLINE template <typename T, typename HashType> bool ExternNodeHashList<T, HashType>::GetFirst(HashType *pHashkey, T *pData) {
	ExternNodeHashNode<T, HashType> *pNode;
	for (int i = 0; i < m_Table_Count; i++){
		if ((pNode = m_pTable[i])){
			*pData = pNode->m_Data;
			*pHashkey = pNode->m_Hashkey;
			return true;
		}
	}
	return false;
}

SHARED_INLINE template <typename T, typename HashType> ExternNodeHashNode<T, HashType> *ExternNodeHashList<T, HashType>::Search(HashType Hashkey) {
	ExternNodeHashNode<T, HashType> *pNode;
	pNode = m_pTable[(Hashkey & 0xFFFF) % m_Table_Count];
	while (pNode){
		if (pNode->m_Hashkey == Hashkey) return pNode;
		pNode = pNode->m_pNext;
	}
	return NULL;
}

SHARED_INLINE template <typename T, typename HashType> ExternNodeHashNode<T, HashType> *ExternNodeHashList<T, HashType>::SearchContinue(ExternNodeHashNode<T, HashType> *pPrevious, HashType Hashkey) {
	ExternNodeHashNode<T, HashType> *pNode;
	if (!pPrevious) return NULL;
	pNode = pPrevious->m_pNext;
	while (pNode){
		if (pNode->m_Hashkey == Hashkey) return pNode;
		pNode = pNode->m_pNext;
	}
	return NULL;
}

SHARED_INLINE template <typename T, typename HashType> int ExternNodeHashList<T, HashType>::GetCount(void) {
	return m_Item_Count;
}

SHARED_INLINE template <typename T, typename HashType> int ExternNodeHashList<T, HashType>::GetTableCount(void) {
	return m_Table_Count;
}

SHARED_INLINE template <typename T, typename HashType> ExternNodeHashNode<T, HashType> *ExternNodeHashList<T, HashType>::GetEntryNode(int table) {
	return m_pTable[table]->m_pNext;
}


SHARED_INLINE template <typename T, typename HashType> ExternNodeHashNode<T, HashType> *ExternNodeHashList<T, HashType>::operator () (HashType HashKey) {
	return Search(HashKey);
}

SHARED_INLINE template <typename T, typename HashType> ExternNodeHashNode<T, HashType> *ExternNodeHashList<T, HashType>::operator () (ExternNodeHashNode<T, HashType> *pPrevious, HashType HashKey) {
	return SearchContinue(pPrevious, HashKey);
}

SHARED_INLINE template <typename T, typename HashType> ExternNodeHashNode<T, HashType> *ExternNodeHashList<T, HashType>::operator () (const char *pString) {
	return Search((HashType)Get_Str_Hash(pString, false));
}

SHARED_INLINE template <typename T, typename HashType> ExternNodeHashNode<T, HashType> *ExternNodeHashList<T, HashType>::operator () (ExternNodeHashNode<T, HashType> *pPrevious, const char *pString) {
	return SearchContinue(pPrevious, (HashType)Get_Str_Hash(pString, false));
}

SHARED_INLINE template <typename T, typename HashType> ExternNodeHashNode<T, HashType> *ExternNodeHashList<T, HashType>::operator () (const wchar_t *pString) {
	return Search((HashType)Get_Str_Hash(pString, false));
}

SHARED_INLINE template <typename T, typename HashType> ExternNodeHashNode<T, HashType> *ExternNodeHashList<T, HashType>::operator () (ExternNodeHashNode<T, HashType> *pPrevious, const wchar_t *pString){
	return SearchContinue(pPrevious, (HashType)Get_Str_HashW(pString, false));
}

struct GenericDLNode : public AutoPoolClass<GenericDLNode, 256> {
	GenericDLNode *m_pPrevious;
	GenericDLNode *m_pNext;	
	void *m_pData;
};

template <typename T> class DList{
private:
	GenericDLNode *m_pHead;
	GenericDLNode *m_pTail;
	GenericDLNode *m_pLast_Added;
public:
	DList(){
		m_pHead = m_pTail = m_pLast_Added = NULL;
	}
	virtual ~DList(){
		Remove_All();
	}
	__inline GenericDLNode *Get_Head(){ return m_pHead; }
	__inline GenericDLNode *Get_Tail(){ return m_pTail; }
	__inline GenericDLNode *Get_Last_Added() { return m_pLast_Added; };
	virtual bool Add_Head(T *pObject){
		GenericDLNode *pTemp;
		if (!pObject) return false;
		pTemp = new GenericDLNode;
		if (m_pHead) m_pHead->m_pPrevious = pTemp;
		pTemp->m_pNext = m_pHead;
		pTemp->m_pPrevious = NULL;
		pTemp->m_pData = pObject;
		m_pHead = pTemp;
		if (!m_pTail) m_pTail = pTemp;
		m_pLast_Added = pTemp;
		return true;
	}
	virtual bool Add_Head(DList<T> &List){
		GenericDLNode *pTemp = NULL, *pNext = NULL, *pCurrent = NULL;
		if(!List.m_pHead) return false;
		pNext = List.m_pHead;
		while (pNext){
			if (pCurrent){
				pTemp = new GenericDLNode;
				pTemp->m_pData = pNext->m_pData;
				pTemp->m_pNext = pCurrent->m_pNext;
				pCurrent->m_pNext = pTemp;
				pTemp->m_pPrevious = pCurrent;
				pCurrent = pTemp;
			} else {
				this->Add_Head((T*)pNext->m_pData);
				pCurrent = m_pHead;
			}
			pNext = pNext->m_pNext;
		}
		return true;
	}
	virtual bool Add_Tail(T *pObject){
		GenericDLNode *pTemp;
		if (!pObject) return false;
		pTemp = new GenericDLNode;
		pTemp->m_pData = pObject;
		pTemp->m_pNext = pTemp->m_pPrevious = NULL;
		
		if (!m_pHead)
			m_pHead = m_pTail = pTemp;
		else {
			if (m_pTail) m_pTail->m_pNext = pTemp;
			pTemp->m_pPrevious = m_pTail;
			m_pTail = pTemp;
		}
		m_pLast_Added = pTemp;
		return true;
	}
	virtual bool Add_Tail(DList<T> &List){
		GenericDLNode *pNext = List.m_pHead;
		if (!pNext) return false;
		while (pNext){
			Add_Tail((T*)pNext->m_pData);
			pNext = pNext->m_pNext;
		}
		return true;
	}
	virtual T *Remove_Head(){
		GenericDLNode *pTemp;
		T *pObject;
		if (!(pTemp = m_pHead)) return NULL;
		m_pHead = m_pHead->m_pNext;
		if (m_pHead) m_pHead->m_pPrevious = NULL;
		pObject = (T*)pTemp->m_pData;
		if (pTemp == m_pTail)
			m_pTail = NULL;
		if (m_pLast_Added == pTemp) m_pLast_Added = NULL;
		delete pTemp;
		return pObject;
	}
	virtual T *Remove_Tail(){
		T *pObject;
		if (!m_pTail) return NULL;
		pObject = (T*)m_pTail->m_pData;
		return Remove(pObject) ? pObject : NULL;
	}
	virtual bool Remove(T *pObject){
		GenericDLNode *pTemp;
		if (!pObject || !(pTemp = m_pHead)) return false;
		if (m_pHead->m_pData == pObject)
			return Remove_Head() ? true : false;
		//pTemp = pTemp->m_pNext;
		while (pTemp = pTemp->m_pNext){
			if (pTemp->m_pData == pObject)
				break;
			//pTemp = pTemp->m_pNext;
		}
		if (!pTemp || !pTemp || pTemp->m_pData != pObject) return false;
		if (pTemp->m_pPrevious) pTemp->m_pPrevious->m_pNext = pTemp->m_pNext;
		if (pTemp->m_pNext) pTemp->m_pNext->m_pPrevious = pTemp->m_pPrevious;

		if (m_pHead == pTemp) m_pHead = pTemp->m_pNext;
		if (m_pTail == pTemp) m_pTail = pTemp->m_pPrevious;

		delete pTemp;
		if (m_pLast_Added == pTemp) m_pLast_Added = NULL;
		return true;

	}
	virtual void Remove_All(){
		GenericDLNode *pTemp, *pTemp2;
		if ((pTemp = m_pHead)){
			while (pTemp){
				pTemp2 = pTemp->m_pNext;
				delete pTemp;
				pTemp = pTemp2;
			}
		}
		m_pHead = m_pTail = NULL;
	}
	virtual bool Insert_Before(T *pObject, T *pBefore){
		GenericDLNode *pTemp, *pCurrent;
		if (!pObject) return false;
		if (!m_pHead || !pBefore)
			return Add_Head(pObject);
		pCurrent = m_pHead;
		while(true) {
			if (!pCurrent || pCurrent->m_pData == pBefore) break;
			pCurrent = pCurrent->m_pNext;
		}
		if (!pCurrent || pCurrent->m_pData != pBefore) return false;
		pTemp = new GenericDLNode;
		pTemp->m_pData = pObject;
		pTemp->m_pNext = NULL;
		pTemp->m_pNext = pCurrent;
		pTemp->m_pPrevious = pCurrent->m_pPrevious;
		if (pTemp->m_pPrevious) pTemp->m_pPrevious->m_pNext = pTemp;
		if (m_pHead == pCurrent) m_pHead = pTemp;
		pCurrent->m_pPrevious = pTemp;
		m_pLast_Added = pTemp;
		return true;

	}
	virtual bool Insert_After(T *pObject, T *pAfter){
		GenericDLNode *pTemp, *pCurrent;
		if (!pObject) return false;
		if (!pAfter || !m_pHead)
			return Add_Head(pObject); //Rather weird after all, but hey!, it works!

		pCurrent = m_pHead;
		while (pCurrent){
			if (pCurrent->m_pData == pAfter) break;
			pCurrent = pCurrent->m_pNext;
		}
		if (!pCurrent || pCurrent->m_pData != pAfter) return false;
		if (pCurrent == m_pTail)
			return Add_Tail(pObject);
		pTemp = new GenericDLNode;
		pTemp->m_pData = pObject;
		pTemp->m_pNext = pCurrent->m_pNext;
		if (pTemp->m_pNext) pTemp->m_pNext->m_pPrevious = pTemp;
		pTemp->m_pPrevious = pCurrent;
		pCurrent->m_pNext = pTemp;
		return true;

	}
	virtual bool Is_Empty(){
		return m_pHead ? false : true;
	}
	virtual int Get_Count(){
		int count = 0;
		GenericDLNode *pTemp;
		if ((pTemp = m_pHead)){
			while (pTemp){
				count++;
				pTemp = pTemp->m_pNext;
			}
		}
		return count;
	}
};


#endif //_LISTS_H_
