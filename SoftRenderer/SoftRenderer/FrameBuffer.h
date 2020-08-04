#pragma once

#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "Vector.h"
#include "Color.h"

template <class T>
class FrameBuffer {
protected:
	size_t width, height;
	size_t size;
	T * buffer;

public:
	FrameBuffer(size_t width, size_t height) : width(width), height(height), size(width * height) {
		buffer = new T[size];
	}
	~FrameBuffer() {
		delete[] buffer;
	}

	inline size_t getWidth() const { return width; }
	inline size_t getHeight() const { return height; }
	inline size_t getSize() const { return size; }
	inline float aspect() const { return (float)getWidth() / getHeight(); }

	void set(size_t x, size_t y, const T & data) { assert(y * width + x < size); buffer[y * width + x] = data; }
	void set(size_t index, const T & data) { assert(index < size); buffer[index] = data; }
	void add(size_t x, size_t y, const T & data) { assert(y * width + x < size); buffer[y * width + x] += data; }
	void add(size_t index, const T & data) { assert(index < size); buffer[index] += data; }

	void clear(size_t x, size_t y) { assert(y * width + x < size); buffer[y * width + x] = T(); }
	void fill(const T & data) { 
		for (size_t i = 0; i < size; i++) buffer[i] = data;
	}
	T get(size_t x, size_t y) const { assert(y * width + x < size); return buffer[y * width + x]; }
	T get(size_t index) const { assert(index < size); return buffer[index]; }
	void get(T & ref, size_t x, size_t y) const { assert(y * width + x < size); ref = buffer[y * width + x]; }
	void get(T & ref, size_t index) const { assert(index < size); ref = buffer[index]; }

	T * operator()(size_t index = 0) { return buffer + index; }
	T * operator()(size_t x, size_t y) { return buffer + (y * width + x); }

	// x, y 在[0, 1)范围内
	T get(float x, float y) const {
		x = Math::fract(x);
		y = Math::fract(y);
		return get((size_t)(x * width) % width, (size_t)(y * height) % height);
	}
	
	// 二维向量(每个元素在[0, 1)范围内)
	T get(const Vector2 & pos) const {
		return get(pos.x, pos.y);
	}
};

typedef FrameBuffer<float> FloatBuffer;
typedef FrameBuffer<int> IntBuffer;
typedef FrameBuffer<RGBColor> ColorBuffer;

shared_ptr<IntBuffer> CreateTexture(const char * filename);

#endif