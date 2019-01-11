/* for Windows */
#include <stdio.h>
#include <windows.h>

#define ptrs_lim 128

static void *heap;


static void *ptrs[ptrs_lim];
static int ptrs_len= 0;



void *parambinder_alloc(int l){
	void *p= 0;

	if( ptrs_len<ptrs_lim && (p= VirtualAlloc(NULL,l, MEM_COMMIT,PAGE_EXECUTE_READWRITE)) ){
		ptrs[ptrs_len++]= p;
	}else{
		int i;
		for(; i<ptrs_len; ++i)
			if(ptrs[i]) VirtualFree(ptrs[i],1,MEM_DECOMMIT);

		fputs("error: object was not allocated.\n", stderr);
		exit(1);
	}

	return p;
}

void *parambinder_free(void *p){
	if(p){
		int i= 0;
		for(; i<ptrs_len && ptrs[i] != p; ++i);
		if( ptrs_len<=i ){
			fprintf(stderr, "error: invalid pointer(%p) was deallocated.\n", p);
		}else{
			VirtualFree(p,1,MEM_DECOMMIT);
			for(--ptrs_len; i<ptrs_len; ++i)
				ptrs[i]= ptrs[i+1];
		}
	}
	return 0;
}

