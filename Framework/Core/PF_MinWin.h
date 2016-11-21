#pragma once
#pragma warning(disable : 4467)


#ifdef _WINDOWS_
	#pragma message ("Windows.h used instead of or before PF_MinWin.h")
#endif

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

// Hackery to try and please the UCRT headers and whatnot even though we don't use them
#if defined(__cplusplus)
	#define __midl 1
	#undef	_X86_
#endif

typedef unsigned char		byte;
typedef int					errno_t;
typedef unsigned long long	size_t;

//using size_t	= unsigned long long;

/*int			_wcsicmp(const wchar_t* string1, const wchar_t* string2);
size_t		wcslen	(const wchar_t* str);
wchar_t*	wcscpy	(wchar_t* dst, const wchar_t* src);
errno_t		wcscpy_s(wchar_t* strDestination, size_t numberOfElements, const wchar_t* strSource);
wchar_t*	wcsrchr	(const wchar_t* str, wchar_t c);
wchar_t*	wcschr	(const wchar_t* str, wchar_t c);*/

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

/*#define NOGDICAPMASKS			// CC_*, LC_*, PC_*, CP_*, TC_*, RC_
//#define NOVIRTUALKEYCODES		// VK_*
//#define NOWINMESSAGES			// WM_*, EM_*, LB_*, CB_*
//#define NOWINSTYLES			// WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
//#define NOSYSMETRICS			// SM_*
#define NOMENUS				// MF_*
//#define NOICONS				// IDI_*
//#define NOKEYSTATES			// MK_*
//#define NOSYSCOMMANDS			// SC_*
#define NORASTEROPS			// Binary and Tertiary raster ops
//#define NOSHOWWINDOW			// SW_*
#define OEMRESOURCE				// OEM Resource values
#define NOATOM					// Atom Manager routines
#define NOCLIPBOARD			// Clipboard routines
// #define NOCOLOR				// Screen colors
#define NOCTLMGR				// Control and Dialog routines
#define NODRAWTEXT				// DrawText() and DT_*
#define NOGDI					// All GDI #defines and routines
#define NOKERNEL				// All KERNEL #defines and routines
//#define NOUSER				// All USER #defines and routines
//#define NONLS					// All NLS #defines and routines
//#define NOMB					// MB_* and MessageBox()
#define NOMEMMGR				// GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE				// typedef METAFILEPICT
#define NOMINMAX				// Macros min(a,b) and max(a,b)
//#define NOMSG					// typedef MSG and associated routines
#define NOOPENFILE				// OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL				// SB_* and scrolling routines
#define NOSERVICE				// All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND					// Sound driver routines
#define NOTEXTMETRIC			// typedef TEXTMETRIC and associated routines
#define NOWH					// SetWindowsHook and WH_*
// #define NOWINOFFSETS			// GWL_*, GCL_*, associated routines
#define NOCOMM					// COMM driver routines
#define NOKANJI					// Kanji support stuff.
#define NOHELP					// Help engine interface.
#define NOPROFILER				// Profiler interface.
#define NODEFERWINDOWPOS		// DeferWindowPos routines
#define NOMCX					// Modem Configuration Extensions
#define NOCRYPT
#define NOTAPE
#define NOIMAGE
#define NOPROXYSTUB
#define NORPC*/

#if defined (_MSC_VER)
	#undef _MSC_VER
	#define _MSC_VER 1299
#endif


//#define CINTERFACE
struct IUnknown;
#include <Windows.h>


// Disgusting Windows header macro hackery
#undef GetCommandLine
#undef GetCurrentDirectory
#undef GetUserName
#undef CreateMutex
#undef WriteConsole
#undef MessageBox