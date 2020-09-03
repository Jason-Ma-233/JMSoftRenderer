#pragma once

#include "../Core/Vector.h"
#include "../Core/Color.h"


template <class T>
class FrameBuffer {
protected:
	size_t width, height;
	size_t size;
	float aspect;
	T* buffer;

public:
	FrameBuffer(size_t width = 2, size_t height = 2) : width(width), height(height), size(width* height), aspect((float)width / height) {
		buffer = new T[size];
	}
	~FrameBuffer() {
		delete[] buffer;
	}

	inline size_t get_width() const { return width; }
	inline size_t get_height() const { return height; }
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
		int x = u, y = v;// min point
		int x2 = x + 1, y2 = y + 1;// max point
		if (x<0 || y<0 || x>width - 1 || y>height - 1)return 0;
		x2 = x2 == width ? 0 : x2, y2 = y2 == height ? 0 : y2;

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
IntBuffer& DownSample(IntBuffer& buffer) {
	IntBuffer newBuffer = IntBuffer(buffer.get_width() * 0.5f, buffer.get_height() * 0.5f);
	for (size_t x = 0; x < newBuffer.get_width(); x++)
	{
		for (size_t y = 0; y < newBuffer.get_height(); y++) {
			RGBColor x1 = RGBColor(buffer.tex2DScreenSpace(x * 2, y * 2));
			RGBColor x2 = RGBColor(buffer.tex2DScreenSpace(x * 2 + 1, y * 2));
			RGBColor y1 = RGBColor(buffer.tex2DScreenSpace(x * 2, y * 2 + 1));
			RGBColor y2 = RGBColor(buffer.tex2DScreenSpace(x * 2 + 1, y * 2 + 1));
			newBuffer.set(x, y, (x1 * 0.25f + x2 * 0.25f + y1 * 0.25f + y2 * 0.25f).toRGBInt());
		}
	}
	return newBuffer;
}

class MipMap {
private:
	IntBuffer maps[5];

public:
	MipMap(IntBuffer& buffer) {
		maps[0] = buffer;
		maps[1] = DownSample(buffer);
		maps[2] = DownSample(maps[1]);
		maps[4] = DownSample(maps[3]);
		maps[3] = DownSample(maps[2]);
	}
	~MipMap() {}

	IntBuffer& operator[](size_t mipmapLevel) { return maps[Math::clamp(mipmapLevel, 0, 4)]; }
};

