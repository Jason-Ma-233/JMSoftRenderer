#include "Pipeline.h"
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
	RGBColor c;
	int rs = currentTexture ? renderState : renderState & (~TEXTURE);
	rs = currentShadeFunc ? rs : rs & (~SHADING);
	float invW = 1.f / screenWidth, invH = 1.f / screenHeight;
	Vector3 pos;
	omp_set_lock(locks + scanline.y);
	for (int x = x0; x <= x1; x++) {
		float rhw = vi.rhw;
		if (rhw >= zbPtr[x]) {  // 使用Z-buffer判断深度是否满足
			v = vi * (1.0f / rhw);
			if (rs & SHADING) {
				pos = vi.point, pos.x *= invW, pos.y *= invH;
				if (currentShadeFunc(c, pos, v.color, v.normal.NormalizedVector(), currentTexture, v.texCoord)) {
					fbPtr[x] = c.toRGBInt();
					zbPtr[x] = rhw;
				}
			}
			else {
				if (rs & TEXTURE) {
					c.setRGBInt(currentTexture->get(v.texCoord));
					if (rs & COLOR) c *= v.color;
				}
				else if (rs & COLOR) {
					c = v.color;
				}
				fbPtr[x] = c.toRGBInt();
				zbPtr[x] = rhw;
			}
		}
		vi += scanline.step;
	}
	omp_unset_lock(locks + scanline.y);
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


void Pipeline::render(const Scene& scene) {
	renderBuffer.fill(clearColor.toRGBInt());
	ZBuffer.fill(0.0f);

	Matrix VPMatrix = scene.view * scene.projection;


	Vector4 c0, c1, c2;
	Vector3 p0, p1, p2;
	VPMatrix.apply(scene.triangle.vertex[0], c0);
	VPMatrix.apply(scene.triangle.vertex[1], c1);
	VPMatrix.apply(scene.triangle.vertex[2], c2);

	transformHomogenize(c0, p0);
	transformHomogenize(c1, p1);
	transformHomogenize(c2, p2);

	TVertex v0, v1, v2;
	SplitedTriangle st;
	v0.point = p0;
	v1.point = p1;
	v2.point = p2;

	v0.init_rhw(c0.w);
	v1.init_rhw(c1.w);
	v2.init_rhw(c2.w);


}