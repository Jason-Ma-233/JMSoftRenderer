#pragma once

#include "Matrix.h"
#include "FrameBuffer.h"
#include "Scene.h"
#include "Primitives.h"

#include <omp.h>

class Pipeline {
public:

private:
	////          缓冲区Buffer          ////
	IntBuffer& renderBuffer;   // 渲染缓冲区
	FloatBuffer ZBuffer;        // Z Buffer

	const int screenWidth;
	const int screenHeight;

	////          当前渲染设置          ////
	RGBColor clearColor;        // 清除颜色

	////       当前渲染的状态变量       ////
	shared_ptr<IntBuffer> currentTexture;   // 当前Mesh使用的纹理
	//ShadeFunc currentShadeFunc;             // 当前Mesh使用的着色函数

	// 画像素点(会检查越界)
	void drawPixel(int x, int y, const RGBColor& color);
	// 光栅化扫描线
	void rasterizeScanline(Scanline& scanline);
	// 切割三角形(任意三角形切为平顶三角形和平底三角形)
	void triangleSpilt(SplitedTriangle& st, const TVertex* v0, const TVertex* v1, const TVertex* v2);
	// 根据y值将平底（顶）三角形转变为扫描线数据
	void rasterizeTriangle(const SplitedTriangle& st);

	// 判断点是否在CVV里面,返回标识位置的码,用于视锥裁剪
	int checkCVV(const Vector4& v);
	// 坐标归一化,并转换到屏幕空间
	void transformHomogenize(const Vector4& src, Vector3& dst);


public:
	Pipeline(IntBuffer& renderBuffer);
	~Pipeline();

	// 设置清除颜色
	void setClearColor(RGBColor clearColor) { this->clearColor = clearColor; }

	// 渲染一帧
	void render(const Scene& scene);
};
