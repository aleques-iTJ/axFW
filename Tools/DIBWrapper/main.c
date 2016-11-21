#pragma once
#include <Windows.h>

// A simple C interface around a Win32 DIB Section
// Used for fast bitmap drawing in the tools because reasons
typedef struct
{
	// What must be provided
	HWND	Hwnd;
	int		Width;
	int		Height;

	// Public
	void*	Data;

	// What's handled internally
	HDC		hdc;
	HDC		hdcMem;
	HBITMAP dib;
	HBITMAP hbmOld;
	int		size;
}DIBInfo;

/*void* __cdecl memset(void* ptr, int value, size_t num)
{
	char* dst = (char*)(ptr);

	size_t i = 0;
	for (; i + 4 <= num; i += 4)
		*(unsigned int*)&dst[i] = *(unsigned int*)&value;

	for (; i <= num; i++)
		dst[i] = (char)value;

	return dst;
}*/

__declspec(dllexport) void CreateDIB(DIBInfo* dibInfo)
{
	BITMAPINFO bi;
	memset(&bi, 0, sizeof(BITMAPINFO));

	bi.bmiHeader.biSize		= sizeof(BITMAPINFO);
	bi.bmiHeader.biWidth	= dibInfo->Width;
	bi.bmiHeader.biHeight	= -dibInfo->Height;
	bi.bmiHeader.biPlanes	= 1;
	bi.bmiHeader.biBitCount = 32;

	dibInfo->size	= (dibInfo->Width * dibInfo->Height) * 4;

	dibInfo->hdc	= GetDC(dibInfo->Hwnd);
	dibInfo->hdcMem = CreateCompatibleDC(dibInfo->hdc);
	dibInfo->dib	= CreateDIBSection(dibInfo->hdc, &bi, DIB_RGB_COLORS, (void**)&dibInfo->Data, 0, 0);
	dibInfo->hbmOld = (HBITMAP)SelectObject(dibInfo->hdcMem, dibInfo->dib);
}

__declspec(dllexport) void DestroyDIB(DIBInfo* dibInfo)
{
	SelectObject(dibInfo->hdcMem, dibInfo->hbmOld);
	DeleteObject(dibInfo->dib);				// DIB
	DeleteDC(dibInfo->hdcMem);				// MemDC
	ReleaseDC(dibInfo->Hwnd, dibInfo->hdc);	// DC
}

__declspec(dllexport) void DrawDIB(DIBInfo* dibInfo)
{
	BitBlt(dibInfo->hdc, 0, 0, dibInfo->Width, dibInfo->Height, dibInfo->hdcMem, 0, 0, SRCCOPY);
	memset(dibInfo->Data, 0, dibInfo->size);
}