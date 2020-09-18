#pragma once

#include "../Core/Vector.h"
#include "../Core/Color.h"


template <class T>
class FrameBuffer {
protected:
	size_t width, height;
	float texelSizeX, texelSizeY;// 1 / wieth height
	size_t size;
	float aspect;
	T* buffer;

public:
	FrameBuffer(size_t width = 2, size_t height = 2) :
		width(width), height(height),
		texelSizeX(1.0f / width), texelSizeY(1.0f / height),
		size(width* height),
		aspect((float)width / height) {
		buffer = new T[size];
	}
	~FrameBuffer() {
		delete[] buffer;
	}

	inline size_t get_width() const { return width; }
	inline size_t get_height() const { return height; }
	inline float get_texelSizeX() const { return texelSizeX; }
	inline float get_texelSizeY() const { return texelSizeY; }
	inline size_t get_size() const { return size; }
	inline float get_aspect() const { return aspect; }

	inline void set(size_t x, size_t y, const T& data) { assert(y * width + x < size); buffer[y * width + x] = data; }
	inline void set(size_t index, const T& data) { assert(index < size); buffer[index] = data; }
	inline void add(size_t x, size_t y, const T& data) { assert(y * width + x < size); buffer[y * width + x] += data; }
	inline void add(size_t index, const T& data) { assert(index < size); buffer[index] += data; }

	inline void clear(size_t x, size_t y) { assert(y * width + x < size); buffer[y * width + x] = T(); }
	inline void fill(const T& data) {
		for (size_t i = 0; i < size; i++) buffer[i] = data;
	}
	inline T get(size_t x, size_t y) const { assert(y * width + x < size); return buffer[y * width + x]; }
	inline T get(size_t index) const { assert(index < size); return buffer[index]; }
	inline void get(T& ref, size_t x, size_t y) const { assert(y * width + x < size); ref = buffer[y * width + x]; }
	inline void get(T& ref, size_t index) const { assert(index < size); ref = buffer[index]; }

	T* operator()(size_t index = 0) { return buffer + index; }
	T* operator()(size_t x, size_t y) { return buffer + (y * width + x); }

	// x, y 在[0, 1)范围内
	inline T get(float x, float y) const {
		x = Math::fract(x);
		y = Math::fract(y);
		return get((size_t)(x * width) % width, (size_t)(y * height) % height);
	}

	// 二维向量(每个元素在[0, 1)范围内)
	inline T get(const Vector2& pos) const {
		return get(pos.x, pos.y);
	}

	// texture sampling
	T tex2DScreenSpace(float u, float v) {
		int x = (int)u % width, y = (int)v % width;// min point
		int x2 = (x + 1) % width, y2 = (y + 1) % width;// max point

		auto leftBottom = buffer[y * width + x],
			leftTop = buffer[y2 * width + x],
			rightBottom = buffer[y * width + x2],
			rightTop = buffer[y2 * width + x2];
		auto top = Math::lerp(leftTop, rightTop, x2 - x);
		auto bottom = Math::lerp(leftBottom, rightBottom, x2 - x);

		return Math::lerp(bottom, top, y2 - y);
	}

	T tex2D(float u, float v) {
		return tex2DScreenSpace(u * width, (1 - v) * height);
	}
};

typedef FrameBuffer<float> FloatBuffer;
typedef FrameBuffer<int> IntBuffer;
typedef FrameBuffer<RGBColor> ColorBuffer;

shared_ptr<IntBuffer> CreateTexture(const char* filename);
shared_ptr<IntBuffer> DownSample(shared_ptr<IntBuffer>& buffer);

class MipMap {
private:
	vector<shared_ptr<IntBuffer>> maps;

public:
	MipMap() { maps.clear(); maps.push_back(nullptr); }
	MipMap(shared_ptr<IntBuffer>& buffer) {
		maps.clear();
		maps.push_back(buffer);
		if (buffer != nullptr) {
			while (maps[maps.size() - 1]->get_width() > 1)
			{
				maps.push_back(DownSample(maps[maps.size() - 1]));
			}
		}
	}
	~MipMap() {}

	inline RGBColor& SampleMipmap(Vector2& uv, Vector2& dx, Vector2& dy, int levelOffset = 0) {
		/*
		float px = maps[0]->get_texelSizeX() * (abs(dx.x) + abs(dx.y));
		float py = maps[0]->get_texelSizeY() * (abs(dy.x) + abs(dy.y));
		size_t lod = (int)Math::clamp(0.5f * log2(MAX(px * px, py * py)), 0, maps.size() - 1);
		*/
		float px = maps[0]->get_width() * (abs(dx.x) + abs(dx.y));
		float py = maps[0]->get_height() * (abs(dy.x) + abs(dy.y));
		size_t lod = (int)Math::clamp(log2(MAX(px, py)) + levelOffset, 0, maps.size() - 1);
		if (lod > 1)
			lod = lod;
		//lod = Math::clamp(lod, 0, maps.size() - 1);
		//return RGBColor().setRGBInt(maps[lod]->tex2D(uv.x, uv.y));
		return RGBColor((lod - 1.0f)*0.5f);
	}
	inline bool isEmpty() { return maps[0] == nullptr; }
	//IntBuffer& operator[](size_t mipmapLevel) { return maps[Math::clamp(mipmapLevel, 0, 4)]; }
};

