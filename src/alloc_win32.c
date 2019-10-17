/* for Windows */
#include <stdio.h>
#include <windows.h>

#ifndef HEAP_SIZE
#define HEAP_SIZE 0x10000
#endif

static CRITICAL_SECTION cs;
static HANDLE heap;


int parambind_i_init()
{ 
	heap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, HEAP_SIZE, HEAP_SIZE);

	if (heap != NULL)
	{
		InitializeCriticalSection(&cs);

		return 1;
	}
	else
		return 0; 
}

int parambind_i_clean() 
{
	if (heap == NULL) return 1;

	EnterCriticalSection(&cs);

	HeapDestroy(heap);
	heap = NULL;

	DeleteCriticalSection(&cs);

	return 1; 
}

void *parambind_i_alloc(size_t size){
	if (heap == NULL) return NULL;

	EnterCriticalSection(&cs);

	void *p = HeapAlloc(heap, HEAP_NO_SERIALIZE, size);

	LeaveCriticalSection(&cs);

	return p;
}

void *parambind_i_free(void *p){
	if (heap == NULL && !p) return NULL;

	EnterCriticalSection(&cs);

	HeapFree(heap, HEAP_NO_SERIALIZE, p);

	LeaveCriticalSection(&cs);

	return NULL;
}

