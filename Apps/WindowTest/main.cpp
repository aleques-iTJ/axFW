#include "bnr.h"


class TestApp
{
public:
	TestApp ();
	~TestApp();


	void Run();

private:
	Window* window;
};

TestApp::TestApp()
{
	window = new Window;
}

TestApp::~TestApp()
{
	delete window;
}


// Events
bool OnKeyPress(int8 key, bool isDown)
{
	String text = "KeyPress: ";
	text << key;

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


struct Grapes
{
	int Test() { return fart; }

	int fart = 32;
};

// Main loop
void TestApp::Run()
{
	window->Create("Test window", 256, 256, WindowFlags::Normal);
	window->OnKeyPress	= OnKeyPress;
	window->OnFocus		= OnFocus;
	window->OnMouseMove = OnMouseMove;
	window->OnResize	= OnResize;
	

	float64 accumulator		= 0.0;
	int32	secondsElapsed	= 0;

	while (window->GetEvents())
	{
		Time::Update();
		accumulator += Time::GetDeltaAsSeconds();


		if (accumulator > 1.0)
		{
			String text;
			text << "Seconds elapsed: " << secondsElapsed << "\n";

			System::WriteConsole(text);
			accumulator = 0.0;
			secondsElapsed++;
		}
	}
}


int main()
{
	TestApp app;
	app.Run();

	return 0;
}