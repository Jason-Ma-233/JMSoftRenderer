#include "Window.h"
#include "Define.h"

using namespace std;

int	main(void) {

	Window window(1280, 720, _T("[ JM Soft Renderer ] "));

	while (window.is_run())
	{
		window.update();
		Sleep(1);
	}

	return 0;
}