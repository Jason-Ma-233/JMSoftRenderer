#pragma once

#include "Vector.h"
#include "Color.h"
#include "FrameBuffer.h"

typedef Vector2 TexCoord;

struct Vertex {
	Vector3 point;
	RGBColor color;
	TexCoord texCoord;
	Vector3 normal;
};

// 着色函数
typedef function<
	bool(RGBColor& out, const Vector3& pos, const RGBColor& color, const Vector3& normal,
		const shared_ptr<IntBuffer>& texture, const TexCoord& texCoord)
> ShadeFunc;

// 三角Mesh
struct Mesh {
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	shared_ptr<IntBuffer> texture;
	ShadeFunc shadeFunc;
};

// 带透视矫正的插值顶点
struct TVertex {
	Vector3 point;
	RGBColor color;
	TexCoord texCoord;
	Vector3 normal;
	float rhw;// view 1/Z

	TVertex() {}
	TVertex(const Vertex& vertex) : color(vertex.color), texCoord(vertex.texCoord) {}
	TVertex(const Vector3& point, const RGBColor& color, TexCoord texCoord, const Vector3& normal, float rhw) :
		point(point), color(color), normal(normal), rhw(rhw), texCoord(texCoord) {}

	void init_rhw(float w) {
		rhw = 1.0f / w;
		color *= rhw;
		texCoord *= rhw;
		normal *= rhw;
	}
	TVertex operator+ (const TVertex& vertex) const {
		return TVertex{
			point + vertex.point,
			color + vertex.color,
			texCoord + vertex.texCoord,
			normal + vertex.normal,
			rhw + vertex.rhw
		};
	}
	TVertex& operator+= (const TVertex& vertex) {
		point += vertex.point;
		color += vertex.color;
		texCoord += vertex.texCoord;
		normal += vertex.normal;
		rhw += vertex.rhw;
		return *this;
	}
	TVertex operator- (const TVertex& vertex) const {
		return TVertex{
			point - vertex.point,
			color - vertex.color,
			texCoord - vertex.texCoord,
			normal - vertex.normal,
			rhw - vertex.rhw,
		};
	}
	TVertex& operator-= (const TVertex& vertex) {
		point -= vertex.point;
		color -= vertex.color;
		texCoord -= vertex.texCoord;
		normal -= vertex.normal;
		rhw -= vertex.rhw;
		return *this;
	}
	TVertex operator* (float k) const {
		return TVertex{
			point * k,
			color * k,
			texCoord * k,
			normal * k,
			rhw * k,
		};
	}
	TVertex& operator*= (float k) {
		point *= k;
		color *= k;
		texCoord *= k;
		normal *= k;
		rhw *= k;
		return *this;
	}

};

// 单根扫描线(横向)
struct Scanline {
	TVertex v0, step;
	int x0, x1, y;
};

// 切割后的三角形组
struct SplitedTriangle {
	// 切割后的三角形种类
	// 00:无 01:平顶 10:平底 11:平顶+平底
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
