#include "Window.h"
#include "Pipeline.h"

using namespace std;

int	main(void) {

	IntBuffer colorBuffer(1280, 720);
	Pipeline pipeline(colorBuffer);
	Window window(colorBuffer.get_width(), colorBuffer.get_height(), _T("[ JM Soft Renderer ] "));

	float aspect = colorBuffer.aspect();
	Scene scene;
	scene.addTriangle(1.0f);

	while (window.is_run())
	{
		scene.clear();
		scene.setPerspective(60.0f, aspect, 0.1f, 1000.0f);
		pipeline.render(scene);

		window.update();
	}

	return 0;
}