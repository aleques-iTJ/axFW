#include <threads.h>
#include "Core/PF_MinWin.h"


int	tss_set(tss_t tss_id, void* val)
{
	BOOL ret = TlsSetValue(tss_id, val);

	if (ret == 0)
	{
		return thrd_error;
	}

	return thrd_success;
}