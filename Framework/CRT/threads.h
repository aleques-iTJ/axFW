#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "API.inl"


typedef struct mtx_t
{
	union
	{
		void*		ptr;
		uintptr_t	id;
	};
	
	union
	{
		struct  
		{
			uint8_t isValid		: 1;
			uint8_t isPlain		: 1;
			uint8_t isRecursive : 1;
			uint8_t isTimed		: 1;
			uint8_t isLocked	: 1;
		};

		uint8_t flags;
	};

	unsigned char pf_Storage[46];
}mtx_t;


typedef struct thrd_t
{
	union
	{
		void*		ptr;
		uintptr_t	id;
	};
}thrd_t;

typedef uint32_t tss_t;

enum
{
	mtx_plain		= (1 << 4),
	mtx_recursive	= (1 << 5),
	mtx_timed		= (1 << 6)
};

enum
{
	thrd_success,
	thrd_nomem,
	thrd_timedout,
	thrd_busy,
	thrd_error
};


typedef int	(*thrd_start_t)(void* arg);
typedef void(*tss_dtor_t)  (void* arg);
#define thread_local __declspec(thread)

AX_API int		thrd_create	(thrd_t* thread, thrd_start_t func, void* arg);
AX_API thrd_t	thrd_current();
AX_API int		thrd_detach	(thrd_t thread);
AX_API int		thrd_equal	(thrd_t lhs, thrd_t rhs);
AX_API void		thrd_exit	(int res);
AX_API int		thrd_join	(thrd_t thread, int* res);
AX_API int		thrd_sleep	(const struct timespec* time_point, struct timespec* remaining);
AX_API void		thrd_yield	();
 
AX_API int		mtx_init	(mtx_t* mutex, int type);
AX_API int		mtx_lock	(mtx_t* mutex);
AX_API int		mtx_unlock	(mtx_t* mutex);
AX_API void		mtx_destroy	(mtx_t* mutex);

AX_API int		tss_create	(tss_t* tss_key, tss_dtor_t destructor);
AX_API int		tss_set		(tss_t tss_id, void* val);
AX_API void*	tss_get		(tss_t tss_key);
AX_API void		tss_delete	(tss_t tss_id);