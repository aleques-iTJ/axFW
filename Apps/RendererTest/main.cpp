#include "bnr.h"


class TestApp
{
public:
	TestApp();
	~TestApp();


	void Run();

private:
	Window*		window;
	Renderer*	renderer;
};

TestApp::TestApp()
{
	window		= new Window;
	renderer	= new Renderer;
}

TestApp::~TestApp()
{
	delete renderer;
	delete window;	
}


// Events
bool OnKeyPress(int8 key, bool isDown)
{
	String text;
	text << "KeyPress: " << key;

	(isDown == true) ?	text.Append(" down \n") :
						text.Append(" up \n");

	System::WriteConsole(text);
	return true;
}

bool OnMouseMove(int32 ax, int32 ay)
{
	String text = "MouseMove: ";
	text << ax << "x, ";
	text << ay << "y\n";

	System::WriteConsole(text);
	return true;
}

bool OnResize(int32 x, int32 y)
{
	String text = "Resize: ";
	text << x << "x, ";
	text << y << "y\n";

	System::WriteConsole(text);
	return true;
}

bool OnFocus(bool isFocused)
{
	String text;

	(isFocused == true) ?	text = "Gained focus\n" :
							text = "Lost focus\n";

	System::WriteConsole(text);
	return true;
}


// Main loop
void TestApp::Run()
{
	window->Create("Test window", 256, 256, WindowFlags::Normal);
	window->OnKeyPress	= OnKeyPress;
	window->OnFocus		= OnFocus;
	window->OnMouseMove = OnMouseMove;
	window->OnResize	= OnResize;

	auto device = renderer->EnumerateDevices()[0];
	device.Create(640, 480, window->GetHandle());

	/*Memory mem;
	BufferInfo info;
	mem.Size  = 100000;
	info.Type = BufferType::VertexBuffer;

	
	BufferID buf = device.CreateBuffer(&mem, info);*/


	float64 accumulator	= 0.0;
	uint8	colorWarp	= 0;
	while (window->GetEvents())
	{
		Time::Update();
		accumulator += Time::GetDeltaAsMs();

		// Draw to the screen at 1 frame per second
		if (accumulator > 1.0)
		{
			accumulator = 0.0;
			colorWarp++;

			device.Frame(colorWarp);
		}		
	}
}


int main()
{
	TestApp app;
	app.Run();

	return 0;
}