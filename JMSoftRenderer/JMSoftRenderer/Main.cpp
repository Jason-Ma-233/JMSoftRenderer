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

	while (window.is_run())
	{
		scene.clear();
		scene.addTriangle(1.0f);

		window.update();
	}

	return 0;
}