#include <windows.h>
#include "Memory.h"

HANDLE	g_hHeap	= NULL;

SHARED_INLINE size_t SHARED_CALL MemSize(void *pMem) {
#ifdef USE_CRT_MEMORY
	return 0; //Not supported
#else //USE_CRT_MEMORY
	return (size_t)HeapSize(g_hHeap, 0, pMem);
#endif //USE_CRT_MEMORY
}

SHARED_INLINE void * SHARED_CALL Malloc(size_t size) {
#ifdef USE_CRT_MEMORY
	return malloc(size);
#else //USE_CRT_MEMORY
	return HeapAlloc(g_hHeap, 0, size);
#endif //USE_CRT_MEMORY
}

SHARED_INLINE void SHARED_CALL Free(void *pMem) {
#ifdef USE_CRT_MEMORY
	free(pMem);
#else //USE_CRT_MEMORY
	HeapFree(g_hHeap, 0, pMem);
#endif //USE_CRT_MEMORY
}

SHARED_INLINE void *SHARED_CALL ReAlloc(void *pMem, size_t size) {
#ifdef USE_CRT_MEMORY
	return realloc(pMem, size);
#else //USE_CRT_MEMORY
	return HeapReAlloc(g_hHeap, 0, pMem, size);
#endif //USE_CRT_MEMORY
}

SHARED_INLINE void SHARED_CALL InitMemory(void) {
#ifndef USE_CRT_MEMORY
	if (!(g_hHeap = GetProcessHeap()))
		g_hHeap = HeapCreate(0, 8192, 0);
#endif //USE_CRT_MEMORY
}

/*fnAllocator*	g_pAllocator;
fnFreeer*		g_pFreeer;
fnReallocator*	g_pReallocator;*/

SHARED_INLINE void *__cdecl operator new(size_t Size) {
	return g_pAllocator(Size);
}

SHARED_INLINE  void *__cdecl operator new(size_t Size, void *Where) {
	return Where;
}

SHARED_INLINE  void	__cdecl operator delete(void *mem) {
	g_pFreeer(mem);
}

SHARED_INLINE  void	__cdecl operator delete(void *mem, void*){
}

SHARED_INLINE void *__cdecl operator new[](size_t Size) {
	return g_pAllocator(Size);
}

SHARED_INLINE  void *__cdecl operator new[](size_t Size, void *Where) {
	return Where;
}

SHARED_INLINE  void	__cdecl operator delete[](void *mem) {
	g_pFreeer(mem);
}

SHARED_INLINE size_t SHARED_CALL Init_MemSize(void *pMem) {
#ifndef USE_CRT_MEMORY
	g_hHeap = GetProcessHeap();
#endif //USE_CRT_MEMORY
	g_pAllocator = Malloc;
	g_pFreeer = Free;
	g_pReallocator = ReAlloc;
	g_pMemSize = MemSize;
	return g_pMemSize(pMem);
}

SHARED_INLINE void * SHARED_CALL Init_Malloc(size_t size) {
#ifndef USE_CRT_MEMORY
	g_hHeap = GetProcessHeap();
#endif //USE_CRT_MEMORY
	g_pAllocator = Malloc;
	g_pFreeer = Free;
	g_pReallocator = ReAlloc;
	g_pMemSize = MemSize;
	return g_pAllocator(size);
}

SHARED_INLINE void SHARED_CALL Init_Free(void *pMem) {
#ifndef USE_CRT_MEMORY
	g_hHeap = GetProcessHeap();
#endif //USE_CRT_MEMORY
	g_pAllocator = Malloc;
	g_pFreeer = Free;
	g_pReallocator = ReAlloc;
	g_pMemSize = MemSize;
	return g_pFreeer(pMem);
}


SHARED_INLINE void *SHARED_CALL Init_ReAlloc(void *pMem, size_t size) {
#ifndef USE_CRT_MEMORY
	g_hHeap = GetProcessHeap();
#endif //USE_CRT_MEMORY
	g_pAllocator = Malloc;
	g_pFreeer = Free;
	g_pReallocator = ReAlloc;
	g_pMemSize = MemSize;
	return g_pReallocator(pMem, size);
}


/*SHARED_INLINE void SHARED_CALL InitMemory(void) {
#ifndef USE_CRT_MEMORY
	if (!(g_hHeap = GetProcessHeap()))
		g_hHeap = HeapCreate(0, 8192, 0);
#endif //USE_CRT_MEMORY
}*/

pfnAllocator	g_pAllocator	= Init_Malloc;
pfnFreeer		g_pFreeer		= Init_Free;
pfnReallocator	g_pReallocator	= Init_ReAlloc;
pfnMemSize		g_pMemSize		= Init_MemSize;

SHARED_INLINE void SetDefaultMemoryFunctions(void) {
	InitMemory();
	g_pAllocator = Malloc;
	g_pFreeer = Free;
	g_pReallocator = ReAlloc;
}

void Set_Memory_Functions(pfnAllocator _pfnAllocator, pfnFreeer _pfnFreeer, pfnReallocator _pfnReallocator){
	g_pAllocator = _pfnAllocator;
	g_pFreeer = _pfnFreeer;
	g_pReallocator = _pfnReallocator;
}

#ifdef LOG_POOL_DATA
int g_Allocated_Objects = 0;
#endif //LOG_POOL_DATA
