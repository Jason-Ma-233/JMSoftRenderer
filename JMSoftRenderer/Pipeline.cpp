#include "header/Pipeline.h"
#include "header/Shader.h"
#include <algorithm>

void Pipeline::shading(TVertex& v, RGBColor& c, Vector2& dx, Vector2& dy) {
	// Shadowmap sampling
	float shadowAttenuation = 1;
	if (enableShadow) {
		auto clipPos_light = _matrix_light_VP.apply(v.worldPos + v.normal * 0.05f);// normal offset bias
		Vector3 screenPos_light;
		transformHomogenize(clipPos_light, screenPos_light, shadowBuffer.get_width(), shadowBuffer.get_height());
		float shadowZ = shadowBuffer.tex2DScreenSpace(screenPos_light.x, screenPos_light.y);
		//float shadowAttenuation = 1 - Math::clamp((shadowZ - 1.0f / screenPos_light.z - 0.1f) * 2.0f);
		shadowAttenuation = shadowZ - 1.0f / screenPos_light.z > 0.1f ? 0 : 1;
	}

	Vector3 N = v.normal.normalize(),
		V = (-cameraPos - v.worldPos).normalize(),
		L = dirLight.dir;
	float NdotL = Math::clamp(N.dot(L));

	// texture samping
	c = currentColor;
	if (!currentTexture.isEmpty()) c *= currentTexture.SampleMipmap(v.texCoord, dx, dy, mipmapLevelOffset);

	Shader::PhysicallyBasedShading(c, roughness, metallic, N, L, V, NdotL);
	c *= dirLight.intensity * dirLight.color * NdotL * shadowAttenuation;


}

void Pipeline::rasterizeScanline(Scanline& scanline) {
	if (scanline.y < 0 || scanline.y >= targetHeight) return;
	int* fbPtr = renderBuffer(0, scanline.y);
	float* zbPtr = ZBuffer(0, scanline.y);
	int x0 = MAX(scanline.x0, 0), x1 = MIN(scanline.x1, targetWidth - 1);
	TVertex vi = scanline.v0, v;
	float invW = 1.f / targetWidth, invH = 1.f / targetHeight;
	RGBColor c(0.5f, 0.5f, 0.5f);

	//omp_set_lock(locks + scanline.y);
	for (int x = x0; x <= x1; x++) {
		// 透视投影比较rhw，正交则直接比较像素深度
		float rhw = projectionMethod == ProjectionMethod::Perspective ? vi.rhw : 1.0f / vi.point.z;
		float rhw_inv = 1.0f / rhw;
		if (rhw >= zbPtr[x]) {  // 比较深度
			v = vi * rhw_inv;// 线性插值后恢复

			// shading
			shading(v, c, scanline.dx * rhw_inv, scanline.dy * rhw_inv);

			fbPtr[x] = c.toRGBInt();
			zbPtr[x] = rhw;
		}
		vi += scanline.step;// 插值结果分布到每像素
	}
	//omp_unset_lock(locks + scanline.y);

}

void Pipeline::rasterizeShadowMap(Scanline& scanline)
{
	if (scanline.y < 0 || scanline.y >= targetHeight) return;
	int* fbPtr = renderBuffer(0, scanline.y);
	float* zbPtr = shadowBuffer(0, scanline.y);
	int x0 = MAX(scanline.x0, 0), x1 = MIN(scanline.x1, targetWidth - 1);
	TVertex vi = scanline.v0;

	for (int x = x0; x <= x1; x++) {
		float z = 1.0f / vi.point.z;
		if (z >= zbPtr[x]) {
			fbPtr[x] = RGBColor(z * 0.1f, z * 0.1f, z * 0.1f).toRGBInt();
			//fbPtr[x] = RGBColor(vi.worldPos.x, vi.worldPos.y, vi.worldPos.z).toRGBInt();
			zbPtr[x] = z;
		}
		vi += scanline.step;// 插值结果分布到每像素
	}

}

void Pipeline::rasterizeTriangle(const SplitedTriangle& st) {
	if (st.type & SplitedTriangle::FLAT_TOP) {
		int y0 = (int)st.bottom.point.y + 1;
		int y1 = (int)st.left.point.y;
		float yl = st.left.point.y - st.bottom.point.y;
		auto median_left = Math::lerp(st.left, st.bottom, 0.5),
			median_right = Math::lerp(st.right, st.bottom, 0.5);
		auto dx = (median_right.texCoord - median_left.texCoord) / (abs(median_right.point.x - median_left.point.x) + 1);
		auto dy = (Math::lerp(st.left.texCoord, st.right.texCoord, 0.5) - st.bottom.texCoord) / (abs(y1 - y0) + 1);

		for (int y = y0; y <= y1; y++) {
			float factor = (y - st.bottom.point.y) / yl;
			TVertex left = Math::lerp(st.bottom, st.left, factor);
			TVertex right = Math::lerp(st.bottom, st.right, factor);
			Scanline scanline;
			scanline.x0 = (int)left.point.x;
			scanline.x1 = (int)right.point.x;
			scanline.y = y;
			scanline.dx = dx;
			scanline.dy = dy;
			scanline.v0 = left;
			scanline.step = (right - left) * (1.0f / (right.point.x - left.point.x));
			(this->*currentRasterizeScanlineFunc)(scanline);
		}
	}
	if (st.type & SplitedTriangle::FLAT_BOTTOM) {
		int y0 = (int)st.left.point.y + 1;
		int y1 = (int)st.top.point.y;
		float yl = st.top.point.y - st.left.point.y;
		auto median_left = Math::lerp(st.left, st.top, 0.5),
			median_right = Math::lerp(st.right, st.top, 0.5);
		auto dx = (median_right.texCoord - median_left.texCoord) / (abs(median_right.point.x - median_left.point.x) + 1);
		auto dy = (Math::lerp(st.left.texCoord, st.right.texCoord, 0.5) - st.top.texCoord) / (abs(y1 - y0) + 1);

		for (int y = y0; y <= y1; y++) {
			float factor = (y - st.left.point.y) / yl;
			TVertex left = Math::lerp(st.left, st.top, factor);
			TVertex right = Math::lerp(st.right, st.top, factor);
			Scanline scanline;
			scanline.x0 = (int)left.point.x;
			scanline.x1 = (int)right.point.x;
			scanline.y = y;
			scanline.dx = dx;
			scanline.dy = dy;
			scanline.v0 = left;
			scanline.step = (right - left) * (1.0f / (right.point.x - left.point.x));
			(this->*currentRasterizeScanlineFunc)(scanline);
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


void Pipeline::renderTriangle(const Vertex* v[3]) {
	Vector4 clipPos[3];
	Vector3 screenPos[3];
	for (size_t i = 0; i < 3; i++) {
		_matrix_MVP.apply(v[i]->point, clipPos[i]);
	}
	for (size_t i = 0; i < 3; i++)
		transformHomogenize(clipPos[i], screenPos[i], targetWidth, targetHeight);

	// 简单cvv裁剪，三角形全在屏幕外则不渲染
	int cvv[3] = { checkCVV(clipPos[0]), checkCVV(clipPos[1]), checkCVV(clipPos[2]) };
	if (cvv[0] > 0 && cvv[1] > 0 && cvv[2] > 0) return;
	// 背面裁剪
	if (cross(screenPos[1] - screenPos[0], screenPos[2] - screenPos[1]).z <= 0)
		return;

	TVertex tv[3];
	SplitedTriangle st;
	for (size_t i = 0; i < 3; i++) {
		tv[i] = TVertex(
			screenPos[i],
			_matrix_M.apply(v[i]->point),
			RGBColor(v[i]->texCoord.x, v[i]->texCoord.y, 1),
			TexCoord(v[i]->texCoord.x, v[i]->texCoord.y),
			_matrix_M.apply(v[i]->normal),
			1);
		tv[i].init_rhw(clipPos[i].w);
	}
	triangleSpilt(st, &tv[0], &tv[1], &tv[2]);
	rasterizeTriangle(st);
}

void Pipeline::renderMeshes(const Scene& scene)
{
	currentRasterizeScanlineFunc = &Pipeline::rasterizeScanline;
	targetWidth = (int)renderBuffer.get_width();
	targetHeight = (int)renderBuffer.get_height();
	_matrix_M = scene.model;
	_matrix_V = scene.view;
	_matrix_P = scene.projection;
	_matrix_VP = scene.view * scene.projection;
	_matrix_MVP = scene.model * _matrix_VP;
	_matrix_light_VP = scene.view_light * scene.projection_light;
	dirLight = scene.dirLight;
	cameraPos = Vector3(_matrix_V[3][0], _matrix_V[3][1], _matrix_V[3][2]);

	for (auto& mesh : scene.meshes)
	{
		currentShadeFunc = mesh.shadeFunc;
		currentTexture = mesh.texture;
		currentColor = mesh.color;

#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < mesh.indices.size(); i += 3)
		{
			const Vertex* v[3] = {
				&mesh.vertices[mesh.indices[i]],
				&mesh.vertices[mesh.indices[i + 1]],
				&mesh.vertices[mesh.indices[i + 2]]
			};
			renderTriangle(v);
		}
	}
}

void Pipeline::renderShadowMap(const Scene& scene)
{
	currentRasterizeScanlineFunc = &Pipeline::rasterizeShadowMap;
	targetWidth = (int)shadowBuffer.get_width();
	targetHeight = (int)shadowBuffer.get_height();
	_matrix_M = scene.model;
	_matrix_V = scene.view_light;
	_matrix_P = scene.projection_light;
	_matrix_VP = scene.view_light * scene.projection_light;
	_matrix_MVP = scene.model * _matrix_VP;
	_matrix_light_VP = _matrix_VP;

	for (auto& mesh : scene.meshes)
	{
#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < mesh.indices.size(); i += 3)
		{
			const Vertex* v[3] = {
				&mesh.vertices[mesh.indices[i]],
				&mesh.vertices[mesh.indices[i + 1]],
				&mesh.vertices[mesh.indices[i + 2]]
			};
			renderTriangle(v);
		}
	}
}
