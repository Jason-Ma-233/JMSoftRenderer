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
	FrameBuffer(size_t width, size_t height) : width(width), height(height), size(width* height), aspect((float)width / height) {
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
	T tex2D(const float u, const float v) {
		int x = u, y = v;
		x = Math::clamp(x, 0, width - 1);
		y = Math::clamp(y, 0, height - 1);
		return buffer[y * width + x];
	}
};

typedef FrameBuffer<float> FloatBuffer;
typedef FrameBuffer<int> IntBuffer;
typedef FrameBuffer<RGBColor> ColorBuffer;

shared_ptr<IntBuffer> CreateTexture(const char* filename);
