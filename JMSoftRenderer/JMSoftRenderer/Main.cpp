#include "Window.h"
#include "Define.h"
#include "Matrix.h"
#include "FrameBuffer.h"
#include "Scene.h"

using namespace std;

int	main(void) {

	IntBuffer colorBuffer(1280, 720);
	Window window(colorBuffer.get_width(), colorBuffer.get_height(), _T("[ JM Soft Renderer ] "));

	float aspect = colorBuffer.aspect();
	Scene scene;
	scene.addTriangle(1.0f);

	while (window.is_run())
	{
		scene.clear();



		window.update();
	}

	return 0;
}