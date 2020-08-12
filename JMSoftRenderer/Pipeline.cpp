#include "header/Pipeline.h"
#include <algorithm>


Pipeline::Pipeline(IntBuffer& renderBuffer) : renderBuffer(renderBuffer),
screenWidth((int)renderBuffer.get_width()), screenHeight((int)renderBuffer.get_height()),
ZBuffer(renderBuffer.get_width(), renderBuffer.get_height()) {
}

Pipeline::~Pipeline() {
}

int Pipeline::checkCVV(const Vector4& v) {
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

void Pipeline::transformHomogenize(const Vector4& src, Vector3& dst) {
	dst = (Vector3)src;
	dst.x = (dst.x + 1.0f) * screenWidth * 0.5f;
	dst.y = (1.0f - dst.y) * screenHeight * 0.5f;
}

inline void Pipeline::drawPixel(int x, int y, const RGBColor& color) {
	//assert(x >= 0 && x < screenWidth&& y >= 0 && y < screenHeight);
	if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
		renderBuffer.set(x, y, color.toRGBInt());
	else
		printf("drawPixel() Out of bound!");
}

void Pipeline::rasterizeScanline(Scanline& scanline) {
	int* fbPtr = renderBuffer(0, scanline.y);
	float* zbPtr = ZBuffer(0, scanline.y);
	int x0 = MAX(scanline.x0, 0), x1 = MIN(scanline.x1, screenWidth - 1);
	TVertex vi = scanline.v0, v;
	float invW = 1.f / screenWidth, invH = 1.f / screenHeight;
	RGBColor c(0.5f, 0.5f, 0.5f);

	//omp_set_lock(locks + scanline.y);
	for (int x = x0; x <= x1; x++) {
		float rhw = vi.rhw;
		if (rhw >= zbPtr[x]) {  // 比较1/z
			v = vi * (1.0f / rhw);// 线性插值后恢复
			v.normal.normalize();
			v.normal = v.normal * 0.5f + 0.5f;
			c.r = v.normal.x;
			c.g = v.normal.y;
			c.b = v.normal.z;
			fbPtr[x] = c.toRGBInt();
			zbPtr[x] = rhw;
		}
		vi += scanline.step;// 插值结果分布到每像素
	}
	//omp_unset_lock(locks + scanline.y);
}

void Pipeline::rasterizeTriangle(const SplitedTriangle& st) {
	if (st.type & SplitedTriangle::FLAT_TOP) {
		int y0 = (int)st.bottom.point.y + 1;
		int y1 = (int)st.left.point.y;
		float yl = st.left.point.y - st.bottom.point.y;

		for (int y = y0; y <= y1; y++) {
			float factor = (y - st.bottom.point.y) / yl;
			TVertex left = Math::lerp(st.bottom, st.left, factor);
			TVertex right = Math::lerp(st.bottom, st.right, factor);
			Scanline scanline;
			scanline.x0 = (int)left.point.x;
			scanline.x1 = (int)right.point.x;
			scanline.y = y;
			scanline.v0 = left;
			scanline.step = (right - left) * (1.0f / (right.point.x - left.point.x));
			rasterizeScanline(scanline);
		}
	}
	if (st.type & SplitedTriangle::FLAT_BOTTOM) {
		int y0 = (int)st.left.point.y + 1;
		int y1 = (int)st.top.point.y;
		float yl = st.top.point.y - st.left.point.y;

		for (int y = y0; y <= y1; y++) {
			float factor = (y - st.left.point.y) / yl;
			TVertex left = Math::lerp(st.left, st.top, factor);
			TVertex right = Math::lerp(st.right, st.top, factor);
			Scanline scanline;
			scanline.x0 = (int)left.point.x;
			scanline.x1 = (int)right.point.x;
			scanline.y = y;
			scanline.v0 = left;
			scanline.step = (right - left) * (1.0f / (right.point.x - left.point.x));
			rasterizeScanline(scanline);
		}
	}
}

void Pipeline::triangleSpilt(SplitedTriangle& st, const TVertex* v0, const TVertex* v1, const TVertex* v2) {
	// 三角形顶点按照Y坐标排序（v0 <= v1 <= v2）
	if (v0->point.y > v1->point.y) swap(v0, v1);
	if (v0->point.y > v2->point.y) swap(v0, v2);
	if (v1->point.y > v2->point.y) swap(v1, v2);

	// 判断三角形共线
	if (Math::isZero(v0->point.y - v1->point.y) && Math::isZero(v1->point.y - v2->point.y) ||
		Math::isZero(v0->point.x - v1->point.x) && Math::isZero(v1->point.x - v2->point.x)) {
		st.type = SplitedTriangle::NONE;
		return;
	}

	if (Math::isZero(v0->point.y - v1->point.y)) { // 底边Y相等（平底三角形）
		assert(v2->point.y > v0->point.y);
		if (v0->point.x > v1->point.x) swap(v0, v1);

		st.top = *v2;
		st.left = *v0;
		st.right = *v1;
		st.type = SplitedTriangle::FLAT_BOTTOM;
		return;
	}
	else if (Math::isZero(v1->point.y - v2->point.y)) { // 顶边Y相等（平顶三角形）
		assert(v2->point.y > v0->point.y);
		if (v1->point.x > v2->point.x) swap(v1, v2);

		st.bottom = *v0;
		st.left = *v1;
		st.right = *v2;
		st.type = SplitedTriangle::FLAT_TOP;
		return;
	}

	st.top = *v2;
	st.bottom = *v0;
	st.type = SplitedTriangle::FLAT_TOP_BOTTOM;

	float factor = (v1->point.y - v0->point.y) / (v2->point.y - v0->point.y);
	TVertex splitV = Math::lerp(st.bottom, st.top, factor);

	if (splitV.point.x <= v1->point.x) {
		st.left = splitV;
		st.right = *v1;
	}
	else {
		st.left = *v1;
		st.right = splitV;
	}
}


void Pipeline::renderTriangle(const Vertex* v[3], Matrix& transform) {
	Vector4 clipPos[3];
	Vector3 screenPos[3];
	for (size_t i = 0; i < 3; i++) {
		transform.apply(v[i]->point, clipPos[i]);
	}
	for (size_t i = 0; i < 3; i++)
		transformHomogenize(clipPos[i], screenPos[i]);

	int cvv[3] = { checkCVV(clipPos[0]), checkCVV(clipPos[1]), checkCVV(clipPos[2]) };
	//if (cvv[0] && cvv[1] && cvv[2]) return;
	if (cvv[0] || cvv[1] || cvv[2]) return;

	if (cross(screenPos[1] - screenPos[0], screenPos[2] - screenPos[1]).z <= 0)
		return;

	TVertex tv[3];
	SplitedTriangle st;
	for (size_t i = 0; i < 3; i++) {
		tv[i] = TVertex(
			screenPos[i],
			RGBColor(v[i]->texCoord.x, v[i]->texCoord.y, 1),
			TexCoord(v[i]->texCoord.x, v[i]->texCoord.y),
			v[i]->normal,
			1);
		tv[i].init_rhw(clipPos[i].w);
	}
	triangleSpilt(st, &tv[0], &tv[1], &tv[2]);
	rasterizeTriangle(st);
}

void Pipeline::renderMeshes(const Scene& scene)
{
	Matrix VPMatrix = scene.view * scene.projection;
	for (auto& mesh : scene.meshes)
	{
		currentShadeFunc = mesh.shadeFunc;
		currentTexture = mesh.texture;

#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < mesh.indices.size(); i += 3)
		{
			const Vertex* v[3] = {
				&mesh.vertices[mesh.indices[i]],
				&mesh.vertices[mesh.indices[i + 1]],
				&mesh.vertices[mesh.indices[i + 2]]
			};
			renderTriangle(v, VPMatrix);
		}
	}
}
