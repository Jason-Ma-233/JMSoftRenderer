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
	////          ������Buffer          ////
	IntBuffer& renderBuffer;	// ��Ⱦ������
	FloatBuffer ZBuffer;        // Z Buffer
	FloatBuffer shadowBuffer;   // light space Z Buffer

	////          ��ǰ��Ⱦ����          ////
	ProjectionMethod projectionMethod = ProjectionMethod::Perspective;

	////       ��ǰ��Ⱦ��״̬����       ////
	shared_ptr<IntBuffer> currentTexture;						// ��ǰMeshʹ�õ�����
	ShadeFunc currentShadeFunc;									// ��ǰMeshʹ�õ���ɫ����
	RGBColor currentColor;										// ��ǰMesh����ɫ
	void (Pipeline::* currentRasterizeScanlineFunc)(Scanline&);	// ��ǰ��ɨ���߹�դ������ָ��

	Matrix _matrix_M, _matrix_V, _matrix_P, _matrix_VP, _matrix_MVP, _matrix_light_VP;
	Vector3 cameraPos;
	DirLight dirLight;


	int targetWidth;
	int targetHeight;


	// ��դ��ɨ����
	void rasterizeScanline(Scanline& scanline);
	// ��դ��ɨ���ߣ�shadowMap�汾��
	void rasterizeShadowMap(Scanline& scanline);
	// �и�������(������������Ϊƽ�������κ�ƽ��������)
	void triangleSpilt(SplitedTriangle& st, const TVertex* v0, const TVertex* v1, const TVertex* v2);
	// ����yֵ��ƽ�ף�����������ת��Ϊɨ��������
	void rasterizeTriangle(const SplitedTriangle& st);
	// ��һ�������α任���ü�������ɨ����
	void renderTriangle(const Vertex* v[3]);
	void shading(TVertex& v, RGBColor& c);

	// �����ص�(����Խ��)
	inline void drawPixel(int x, int y, const RGBColor& color) {
		if (x >= 0 && x < targetWidth && y >= 0 && y < targetHeight)
			renderBuffer.set(x, y, color.toRGBInt());
		else
			printf("drawPixel() Out of bound!");
	}

	// �жϵ��Ƿ���CVV����,���ر�ʶλ�õ���,������׶�ü�
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

	// �����һ��,��ת������Ļ�ռ�
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
