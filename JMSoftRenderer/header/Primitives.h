#pragma once

#include "../Core/Vector.h"
#include "../Core/Color.h"
#include "FrameBuffer.h"

typedef Vector2 TexCoord;

struct Vertex {
	Vector3 point;
	RGBColor color;
	TexCoord texCoord;
	Vector3 normal;
};

// ��ɫ����
typedef function<
	bool(RGBColor& out, const Vector3& pos, const RGBColor& color, const Vector3& normal,
		const shared_ptr<IntBuffer>& texture, const TexCoord& texCoord)
> ShadeFunc;

// ����Mesh
struct Mesh {
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	MipMap texture;
	ShadeFunc shadeFunc;
	RGBColor color;
};

// ��͸�ӽ����Ĳ�ֵ����
struct TVertex {
	Vector3 point;
	Vector3 worldPos;
	RGBColor color;
	TexCoord texCoord;
	Vector3 normal;
	float rhw;// view 1/Z

	TVertex() {}
	TVertex(const Vertex& vertex) : color(vertex.color), texCoord(vertex.texCoord) {}
	TVertex(const Vector3& point, const Vector3& worldPos, const RGBColor& color, TexCoord texCoord, const Vector3& normal, float rhw) :
		point(point), worldPos(worldPos), color(color), normal(normal), rhw(rhw), texCoord(texCoord) {}

	void init_rhw(float w) {
		rhw = 1.0f / w;
		worldPos *= rhw;
		color *= rhw;
		texCoord *= rhw;
		normal *= rhw;
	}
	TVertex operator+ (const TVertex& vertex) const {
		return TVertex{
			point + vertex.point,
			worldPos + vertex.worldPos,
			color + vertex.color,
			texCoord + vertex.texCoord,
			normal + vertex.normal,
			rhw + vertex.rhw
		};
	}
	TVertex& operator+= (const TVertex& vertex) {
		point += vertex.point;
		worldPos += vertex.worldPos;
		color += vertex.color;
		texCoord += vertex.texCoord;
		normal += vertex.normal;
		rhw += vertex.rhw;
		return *this;
	}
	TVertex operator- (const TVertex& vertex) const {
		return TVertex{
			point - vertex.point,
			worldPos - vertex.worldPos,
			color - vertex.color,
			texCoord - vertex.texCoord,
			normal - vertex.normal,
			rhw - vertex.rhw,
		};
	}
	TVertex& operator-= (const TVertex& vertex) {
		point -= vertex.point;
		worldPos -= vertex.worldPos;
		color -= vertex.color;
		texCoord -= vertex.texCoord;
		normal -= vertex.normal;
		rhw -= vertex.rhw;
		return *this;
	}
	TVertex operator* (float k) const {
		return TVertex{
			point * k,
			worldPos * k,
			color * k,
			texCoord * k,
			normal * k,
			rhw * k,
		};
	}
	TVertex& operator*= (float k) {
		point *= k;
		worldPos *= k;
		color *= k;
		texCoord *= k;
		normal *= k;
		rhw *= k;
		return *this;
	}

};

// ����ɨ����(����)
struct Scanline {
	TVertex v0, step;
	int x0, x1, y;
	Vector2 dy;
};

// �и�����������
struct SplitedTriangle {
	// �и�������������
	// 00:�� 01:ƽ�� 10:ƽ�� 11:ƽ��+ƽ��
	enum TriangleType {
		NONE,
		FLAT_TOP,
		FLAT_BOTTOM,
		FLAT_TOP_BOTTOM
	};

	TVertex top;
	TVertex left, right;
	TVertex bottom;
	TriangleType type;
};
