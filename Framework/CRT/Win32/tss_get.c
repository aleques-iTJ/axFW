#include <threads.h>
#include "Core/PF_MinWin.h"


void* tss_get(tss_t tss_key)
{
	return TlsGetValue(tss_key);
}