#include <threads.h>
#include "Core/PF_MinWin.h"


void tss_delete(tss_t tss_id)
{
	TlsFree(tss_id);
}