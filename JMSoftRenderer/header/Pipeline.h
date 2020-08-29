#pragma once

#include "../Core/Matrix.h"
#include "FrameBuffer.h"
#include "Primitives.h"
#include "Scene.h"

#include <omp.h>

enum ProjectionMethod
{
	Perspective,
	Orthogonal
};

class Pipeline {
	//public:

private:
	////          缓冲区Buffer          ////
	IntBuffer& renderBuffer;	// 渲染缓冲区
	FloatBuffer ZBuffer;        // Z Buffer
	FloatBuffer shadowBuffer;   // light space Z Buffer

	////          当前渲染设置          ////
	ProjectionMethod projectionMethod = ProjectionMethod::Perspective;

	////       当前渲染的状态变量       ////
	shared_ptr<IntBuffer> currentTexture;						// 当前Mesh使用的纹理
	ShadeFunc currentShadeFunc;									// 当前Mesh使用的着色函数
	RGBColor currentColor;										// 当前Mesh的颜色
	void (Pipeline::* currentRasterizeScanlineFunc)(Scanline&);	// 当前的扫描线光栅化函数指针

	Matrix _matrix_M, _matrix_V, _matrix_P, _matrix_VP, _matrix_MVP, _matrix_light_VP;
	Vector3 cameraPos;
	DirLight dirLight;


	int targetWidth;
	int targetHeight;


	// 光栅化扫描线
	void rasterizeScanline(Scanline& scanline);
	// 光栅化扫描线（shadowMap版本）
	void rasterizeShadowMap(Scanline& scanline);
	// 切割三角形(任意三角形切为平顶三角形和平底三角形)
	void triangleSpilt(SplitedTriangle& st, const TVertex* v0, const TVertex* v1, const TVertex* v2);
	// 根据y值将平底（顶）三角形转变为扫描线数据
	void rasterizeTriangle(const SplitedTriangle& st);
	// 对一个三角形变换、裁剪、生成扫描线
	void renderTriangle(const Vertex* v[3]);
	void shading(TVertex& v, RGBColor& c);

	// 画像素点(会检查越界)
	inline void drawPixel(int x, int y, const RGBColor& color) {
		if (x >= 0 && x < targetWidth && y >= 0 && y < targetHeight)
			renderBuffer.set(x, y, color.toRGBInt());
		else
			printf("drawPixel() Out of bound!");
	}

	// 判断点是否在CVV里面,返回标识位置的码,用于视锥裁剪
	inline int checkCVV(const Vector4& v) {
		float w = v.w;
		int check = 0;
		if (v.z < 0.f) check |= 1;
		if (v.z > w)   check |= 2;
		if (v.x < -w)  check |= 4;
		if (v.x > w)   check |= 8;
		if (v.y < -w)  check |= 16;
		if (v.y > w)   check |= 32;
		return check;
	}

	// 坐标归一化,并转换到屏幕空间
	inline void transformHomogenize(const Vector4& src, Vector3& dst, float width, float height) { transformHomogenize((Vector3)src, dst, width, height); }
	inline void transformHomogenize(const Vector3& src, Vector3& dst, float width, float height) {
		dst = Vector3(src);
		dst.x = (dst.x + 1.0f) * width * 0.5f;
		dst.y = (1.0f - dst.y) * height * 0.5f;
	}


public:
	Pipeline(IntBuffer& renderBuffer, size_t shadowMapSize, ProjectionMethod method = ProjectionMethod::Perspective) :
		renderBuffer(renderBuffer),
		targetWidth((int)renderBuffer.get_width()),
		targetHeight((int)renderBuffer.get_height()),
		ZBuffer(renderBuffer.get_width(),
			renderBuffer.get_height()),
		shadowBuffer(shadowMapSize, shadowMapSize),
		projectionMethod(method),
		currentRasterizeScanlineFunc(&Pipeline::rasterizeScanline)
	{}
	~Pipeline() {}

	void clearBuffers(RGBColor clearColor) {
		this->renderBuffer.fill(clearColor.toRGBInt());
		this->ZBuffer.fill(0.0f);
		this->shadowBuffer.fill(0.0f);
	}

	void setProjectionMethod(ProjectionMethod method) { this->projectionMethod = method; }

	void renderMeshes(const Scene& scene);
	void renderShadowMap(const Scene& scene);
};
