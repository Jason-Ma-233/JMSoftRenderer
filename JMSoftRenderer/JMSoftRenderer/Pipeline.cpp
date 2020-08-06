#include "Pipeline.h"
//#include <algorithm>
//
//Pipeline::Pipeline(IntBuffer& renderBuffer) : renderBuffer(renderBuffer),
//screenWidth((int)renderBuffer.get_width()), screenHeight((int)renderBuffer.get_height()),
//ZBuffer(renderBuffer.get_width(), renderBuffer.get_height()) {
//}
//
//Pipeline::~Pipeline() {
//}
//
//void Pipeline::render(const Scene& scene) {
//	renderBuffer.fill(clearColor.toRGBInt());
//	ZBuffer.fill(0.0f);
//
//	Matrix VPMatrix = scene.view * scene.projection;
//
//
//	Triangle triangle;
//	for (size_t i = 0; i < 3; i++)
//	{
//		triangle.vertex[i] = VPMatrix.apply(scene.triangle.vertex[i]);
//	}
//
//
//}