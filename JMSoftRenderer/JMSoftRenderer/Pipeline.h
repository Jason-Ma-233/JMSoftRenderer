#pragma once

#include "Matrix.h"
#include "FrameBuffer.h"
#include "Scene.h"
#include "Primitives.h"

#include <omp.h>

class Pipeline {
public:

private:
	////          ������Buffer          ////
	IntBuffer& renderBuffer;   // ��Ⱦ������
	FloatBuffer ZBuffer;        // Z Buffer

	const int screenWidth;
	const int screenHeight;

	////          ��ǰ��Ⱦ����          ////
	RGBColor clearColor;        // �����ɫ

	////       ��ǰ��Ⱦ��״̬����       ////
	shared_ptr<IntBuffer> currentTexture;   // ��ǰMeshʹ�õ�����
	//ShadeFunc currentShadeFunc;             // ��ǰMeshʹ�õ���ɫ����

	// �����ص�(����Խ��)
	void drawPixel(int x, int y, const RGBColor& color);
	// ��դ��ɨ����
	void rasterizeScanline(Scanline& scanline);
	// �и�������(������������Ϊƽ�������κ�ƽ��������)
	void triangleSpilt(SplitedTriangle& st, const TVertex* v0, const TVertex* v1, const TVertex* v2);
	// ����yֵ��ƽ�ף�����������ת��Ϊɨ��������
	void rasterizeTriangle(const SplitedTriangle& st);

	// �жϵ��Ƿ���CVV����,���ر�ʶλ�õ���,������׶�ü�
	int checkCVV(const Vector4& v);
	// �����һ��,��ת������Ļ�ռ�
	void transformHomogenize(const Vector4& src, Vector3& dst);


public:
	Pipeline(IntBuffer& renderBuffer);
	~Pipeline();

	// ���������ɫ
	void setClearColor(RGBColor clearColor) { this->clearColor = clearColor; }

	// ��Ⱦһ֡
	void render(const Scene& scene);
};
