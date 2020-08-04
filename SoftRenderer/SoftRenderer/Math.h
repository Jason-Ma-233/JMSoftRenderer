#pragma once

#ifndef _MATH_H_
#define _MATH_H_

#include <cmath>

namespace Math {
	const static float Infinity = std::numeric_limits<float>::infinity();

	const static float PI = 3.14159265358979323846f;
	const static float INV_PI = 0.31830988618379067153f;
	const static float DEGREE_TO_RADIUS = PI / 180.f;
	const static float RADIUS_TO_DEGREE = INV_PI * 180.f;

	const static float E_CONSTANT = 2.71828182845904523536f;

	const static float EPS = 1e-6f;

	template <class T1, class T2>
	inline auto _max(const T1 a, const T2 b) {
		return (a > b) ? a : b;
	}

	template <class T1, class T2>
	inline auto _min(const T1 a, const T2 b) {
		return (a < b) ? a : b;
	}

	template <class T1, class T2 = T1, class T3 = T1>
	inline auto clamp(const T1 x, const T2 minValue = 0, const T3 maxValue = 1) {
		return (x < minValue) ? minValue : ((x > maxValue) ? maxValue : x);
	}

	// a, b 可以为Vector或其他支持点乘的类型
	template <class T1, class T2>
	inline T1 lerp(const T1 &a, const T1 &b, const T2 t) {
		return a * (1 - t)  + b * t;
	}

	template <class T>
	inline T smoothStep(const T a, const T b, T x) {
		if (x <= a)
			return 0;
		if (x >= b)
			return 1;
		x = clamp((x - a) / (b - a), 0, 1);
		return x * x * (3 - 2 * x);
	}

	template <class T>
	inline T smootherStep(const T a, const T b, T x) {
		if (x <= a)
			return 0;
		if (x >= b)
			return 1;
		x = clamp((x - a) / (b - a), 0, 1);
		return x * x * x * (x * (x * 6 - 15) + 10);
	}

	template <class T>
	inline bool isInRange(const T a, const T min, const T max) {
		return a >= min && a <= max;
	}

	template <class T>
	inline bool isZero(const T x) {
		return x > -EPS && x < EPS;
	}

	inline int floor(float x) {
		return int(x) - (x >= 0 ? 0 : 1);
	}

	inline int ceil(float x) {
		return int(x) + (x >= 0 ? 1 : 0);
	}

	inline int round(float x) {
		return floor(x + 0.5f);
	}

	inline float fract(float x) {
		return x - std::floor(x);
	}

	inline float fastPow(float a, float b) {
		union {
			float d;
			int x[2];
		} u = { a };
		u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
		u.x[0] = 0;
		return u.d;
	}

	// should be much more precise with large b
	inline float fastPrecisePow(float a, float b) {
		// calculate approximation with fraction of the exponent
		int e = (int)b;
		union {
			float d;
			int x[2];
		} u = { a };
		u.x[1] = (int)((b - e) * (u.x[1] - 1072632447) + 1072632447);
		u.x[0] = 0;

		// exponentiation by squaring with the exponent's integer part
		// float r = u.d makes everything much slower, not sure why
		float r = 1.0;
		while (e) {
			if (e & 1) {
				r *= a;
			}
			a *= a;
			e >>= 1;
		}

		return r * u.d;
	}

	// suitable for x lessThan 5.0
	inline float fastExp(float x) {
		x = 1.0f + x / 512;
		x *= x; x *= x; x *= x; x *= x;
		x *= x; x *= x; x *= x; x *= x;
		x *= x;
		return x;
	}

	inline float fastSin(float x) {
		x = (x - PI / 2) / (2 * PI);
		x -= floor(x);
		x = abs(x * 2 - 1);
		x = smoothStep(0.0f, 1.0f, x) * 2 - 1;
		return x;
	}

	inline float fastCos(float x) {
		x = x / (2 * PI) - 1;
		x -= floor(x);
		x = abs(x * 2 - 1);
		x = smoothStep(0.0f, 1.0f, x) * 2 - 1;
		return x;
	}

	template <class T>
	bool solveQuadratic(const T a, const T b, const T c, T &x0, T &x1) {
		T discr = b * b - 4 * a * c;
		if (discr < 0) return false;
		else if (discr == 0) {
			x0 = x1 = -0.5 * b / a;
		} else {
			T q = (b > 0) ?
				-0.5 * (b + sqrt(discr)) :
				-0.5 * (b - sqrt(discr));
			x0 = q / a;
			x1 = c / q;
		}
		return true;
	}

	template <class T>
	inline T solveCubicSpline(const T r) {
		T u = r;
		for (int i = 0; i < 5; i++)
			u = (11.0 * r + u * u * (6.0 + u * (8.0 - 9.0 * u))) / (4.0 + 12.0 * u * (1.0 + u * (1.0 - u)));
		return u;
	}

	template <class T>
	T cubicFilter(const T x) {
		if (x < 1.0 / 24.0)
			return fastPow(24 * x, 0.25) - 2;
		else if (x < 0.5)
			return solveCubicSpline(24 * (x - 1.0 / 24.0) / 11.0) - 1;
		else if (x < 23.0 / 24.0)
			return 1 - solveCubicSpline(24 * (23.0 / 24.0 - x) / 11.0);
		else
			return 2 - fastPow(24 * (1 - x), 0.25);
	}

	template <class T>
	inline int cutToSqrt(const T a) {
		int r = int(sqrt(a));
		return r * r;
	}

	inline int intHashXY(int x, int y) {
		int key = x * 2731 + y * 1549;
		key += ~(key << 15);
		key ^= (key >> 10);
		return key;
	}
}

// frequently used micro
#define MAX(a,b)  Math::_max(a, b)
#define MIN(a,b)  Math::_min(a, b)

#endif

