/* for generic unsafe systems */
#include <stdio.h>
#include <stdlib.h>

#define ptrs_lim 4096


static void *ptrs[ptrs_lim];
static int ptrs_len = 0;


int parambind_i_init()
{ 
	return 1; 
}
int parambind_i_clean()
{
	for (int i = 0; i < ptrs_len; ++i)
		if (ptrs[i]) free(ptrs[i]);

	return 1;
}


void *parambind_i_alloc(size_t size)
{
	if (ptrs_len >= ptrs_lim) return NULL;
	
	void *p = malloc(size);
	
	if (p) ptrs[ptrs_len++] = p;

	return p;
}

void *parambind_i_free(void *p)
{
	if (!p) return NULL;

	int i;

	for (i = 0; i < ptrs_len && ptrs[i] != p; ++i);

	if (i < ptrs_len)
	{
		free(p);

		for (--ptrs_len; i < ptrs_len; ++i)
			ptrs[i] = ptrs[i + 1];
	}

	return 0;
}

