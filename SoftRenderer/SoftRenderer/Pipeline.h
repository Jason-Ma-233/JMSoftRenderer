#pragma once

#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include "Matrix44.h"
#include "FrameBuffer.h"
#include "Primitives.h"
#include "Scene.h"

#include <omp.h>

class Pipeline {
public:
	// 渲染状态(指示当前的渲染模式)
	enum RenderState { 
		WIREFRAME = 1, 
		COLOR = 2, 
		TEXTURE = 4, 
		COLOR_TEXTURE = COLOR | TEXTURE,
		SHADING = 8 
	};

	// 清除状态(指示渲染前清除哪些buffer)
	enum ClearState {
		CLEAR_NONE = 0,
		CLEAR_COLOR = 1,
		CLEAR_DEPTH = 2,
		CLEAR_COLOR_DEPTH = CLEAR_COLOR | CLEAR_DEPTH
	};

private:
	////          缓冲区Buffer          ////
	IntBuffer & renderBuffer;   // 渲染缓冲区
	FloatBuffer ZBuffer;        // Z Buffer
	omp_lock_t * locks;         // 多线程锁

	const int screenWidth;
	const int screenHeight;

	////          当前渲染设置          ////

	RGBColor clearColor;        // 清除颜色
	RenderState renderState;    // 当前的渲染状态
	ClearState clearState;      // 当前的清除状态

	bool smoothLine;            // 是否开启线条抗锯齿

	////       当前渲染的状态变量       ////

	shared_ptr<IntBuffer> currentTexture;   // 当前Mesh使用的纹理
	ShadeFunc currentShadeFunc;             // 当前Mesh使用的着色函数

	// 画像素点(会检查越界)
	void drawPixel(int x, int y, const RGBColor & color);
	// 光栅化直线(Bresenham's algorithm)
	void rasterizeLine(float x0, float y0, float x1, float y1, RGBColor c0, RGBColor c1);
	// 光栅化反走样直线(Xiaolin Wu's line algorithm)
	void rasterizeLine_antialiasing(float x0, float y0, float x1, float y1, RGBColor c0, RGBColor c1);
	// 光栅化扫描线
	void rasterizeScanline(Scanline & scanline);
	// 切割三角形(任意三角形切为平顶三角形和平底三角形)
	void triangleSpilt(SplitedTriangle & st, const TVertex * v0, const TVertex * v1, const TVertex * v2);
	// 根据y值将平底（顶）三角形转变为扫描线数据
	void rasterizeTriangle(const SplitedTriangle & st);

	// 判断点是否在CVV里面,返回标识位置的码,用于视锥裁剪
	int checkCVV(const Vector4 & v);
	// 坐标归一化,并转换到屏幕空间
	void transformHomogenize(const Vector4 & src, Vector3 & dst);
	// 直线剪裁(Liang–Barsky algorithm)
	bool lineClipping(float & x0, float & y0, float & x1, float & y1);

	// 渲染一条直线
	void renderLine(const Line & line, const Matrix44 & transform);
	// 渲染一个mesh
	void renderMesh(const shared_ptr<Mesh> mesh, const Matrix44 & transform, const Matrix44 & normalMatrix);

public:
	Pipeline(IntBuffer & renderBuffer);
	~Pipeline();

	// 设置渲染状态
	void setRenderState(RenderState state) { this->renderState = state; }
	// 设置清除颜色
	void setClearColor(RGBColor clearColor) { this->clearColor = clearColor; }
	
	// 渲染一帧
	void render(const Scene & scene);
};

#endif