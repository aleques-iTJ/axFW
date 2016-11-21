#pragma once
#include "Core/Containers/StringView.h"
#include "Core/Utility/Delegate.h"


enum_flags(WindowFlags)
enum class WindowFlags
{
	Normal			= 1 << 0,	// Typical window
	FixedSize		= 1 << 1,	// No resize handle / maximize
	Borderless		= 1 << 2,	// No border
	FullScreen		= 1 << 3,	// Enforces window as being the top most window
	LockCursor		= 1 << 4,	// Disallow the cursor from leaving the window
	ForceMovable	= 1 << 5	// Allows the window to be dragged from anywhere
};


// OS Window interface
class Window
{

private:
	using OnCreateEvent		= Delegate<bool(*)()>;
	using OnCloseEvent		= Delegate<bool(*)()>;
	using OnDestroyEvent	= Delegate<bool(*)()>;

	using OnTextEvent		= Delegate<bool(*)(int8 key)>;
	using OnKeyPressEvent	= Delegate<bool(*)(int8 key, bool isDown)>;
	using OnMouseMoveEvent	= Delegate<bool(*)(int32 x, int32 y)>;
	using OnResizeEvent		= Delegate<bool(*)(int32 x, int32 y)>;
	using OnFocusEvent		= Delegate<bool(*)(bool isFocused)>;


//==============================================================================================
// Window setup and destruction
//==============================================================================================

public:
	bool	Create			(StringView title, int32 width, int32 height, WindowFlags flags);
	bool	Destroy			();


//==============================================================================================
// Windows message pump and events
//==============================================================================================

	bool	GetEvents		() const;
	bool	GetEventsWait	() const;
	

//==============================================================================================
// Windows state management and helpers
//==============================================================================================	

	void	SetTitle		(StringView title);
	void	SetFlags		(WindowFlags flags);
	void	SetPosition		(int32 x, int32 y);
	void	SetDimensions	(int32 width, int32 height);

	void	ShowCursor		(bool isVisible);
	void	ShowWindow		(bool isVisible);

	bool	IsMinimized		() const;
	bool	IsMaximized		() const;
	bool	IsValid			() const;

	String	GetTitle		() const;
	void*	GetHandle		() const;


//==============================================================================================
// Event callbacks
//==============================================================================================

	// Set up callback defaults
	OnCreateEvent		OnCreate		= Bind(static_cast<bool(*)()> ([]() { return false; }));
	OnCloseEvent		OnClose			= Bind(static_cast<bool(*)()> ([]() { return false; }));
	OnDestroyEvent		OnDestroy		= Bind(static_cast<bool(*)()> ([]() { return false; }));

	OnTextEvent			OnText			= Bind(static_cast<bool(*)(int8)>			([](int8)			{ return false; }));
	OnKeyPressEvent		OnKeyPress		= Bind(static_cast<bool(*)(int8, bool)>		([](int8, bool)		{ return false; }));
	OnMouseMoveEvent	OnMouseMove		= Bind(static_cast<bool(*)(int32, int32)>	([](int32, int32)	{ return false; }));
	OnResizeEvent		OnResize		= Bind(static_cast<bool(*)(int32, int32)>	([](int32, int32)	{ return false; }));
	OnFocusEvent		OnFocus			= Bind(static_cast<bool(*)(bool)>			([](bool)			{ return false; }));


private:
	String		title		= "axFW Window";

	int32		w_width		= 1280;
	int32		w_height	= 720;
	//int32		w_style		= 0;

	int8		pf_Storage[32];	// This MUST be able to hold WindowPF!
};