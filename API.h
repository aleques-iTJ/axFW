

#if defined (BNR_DYNAMIC)
	#if defined (__cplusplus)
		#define AX_API extern "C" __declspec(dllexport) 
	#else
		#define AX_API __declspec(dllexport)
	#endif
#else
	#if defined (__cplusplus)
		#define AX_API extern "C" 
	#else
		#define AX_API
	#endif
#endif