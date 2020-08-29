#pragma once


#include "Define.h"

class Vector2 {
public:
	float x, y;
	Vector2() :x(0), y(0) {};
	Vector2(float x, float y) :x(x), y(y) {};
	Vector2(const Vector2& v) :x(v.x), y(v.y) {};
	~Vector2() {};

	inline float operator [] (uint8_t i) const { return (&x)[i]; }
	inline float& operator [] (uint8_t i) { return (&x)[i]; }

	inline Vector2 operator-() const {
		return Vector2(-x, -y);
	}

	inline bool operator==(const Vector2& v) const {
		return x == v.x && y == v.y;
	}

	inline bool operator!=(const Vector2& v) const {
		return x != v.x || y != v.y;
	}

	inline bool isZero() const {
		return Math::isZero(x) && Math::isZero(y);
	}

	inline Vector2 operator+(const Vector2& v) const {
		return Vector2(x + v.x, y + v.y);
	}
	inline Vector2 operator-(const Vector2& v) const {
		return Vector2(x - v.x, y - v.y);
	}

	inline float operator*(const Vector2& v) const {
		return x * v.x + y * v.y;
	}

	inline Vector2 operator*(const float a) const {
		return Vector2(x * a, y * a);
	}
	inline Vector2 operator/(const float a) const {
		float oneOverA = 1.0f / a;
		return Vector2(x * oneOverA, y * oneOverA);
	}

	inline Vector2& operator+=(const Vector2& v) {
		x += v.x, y += v.y;
		return *this;
	}
	inline Vector2& operator-=(const Vector2& v) {
		x -= v.x, y -= v.y;
		return *this;
	}
	inline Vector2& operator*=(const Vector2& v) {
		x *= v.x, y *= v.y;
		return *this;
	}
	inline Vector2& operator*=(const float a) {
		x *= a, y *= a;
		return *this;
	}
	inline Vector2& operator/=(const float a) {
		float oneOverA = 1.0f / a;
		x *= oneOverA, y *= oneOverA;
		return *this;
	}

	inline Vector2& normalize() {
		float magSq = x * x + y * y;
		if (magSq > .0f) {
			float oneOverMag = 1.0f / sqrt(magSq);
			x *= oneOverMag, y *= oneOverMag;
		}
		return *this;
	}

	inline Vector2 NormalizedVector() {
		float magSq = x * x + y * y;
		if (magSq > .0f) {
			float oneOverMag = 1.0f / sqrt(magSq);
			return Vector2(x * oneOverMag, y * oneOverMag);
		}
		return Zero();
	}

	inline float length() const {
		return float(sqrt(x * x + y * y));
	}
	inline float lengthSqr() const {
		return float(x * x + y * y);
	}

	inline friend float distance(const Vector2& v1, const Vector2& v2) {
		float dx = v1.x - v2.x;
		float dy = v1.x - v2.x;
		return sqrt(dx * dx + dy * dy);
	}
	inline friend float distanceSqr(const Vector2& v1, const Vector2& v2) {
		float dx = v1.x - v2.x;
		float dy = v1.x - v2.x;
		return dx * dx + dy * dy;
	}

	// 标量乘法
	inline friend Vector2 operator*(float k, const Vector2& v) {
		return Vector2(k * v.x, k * v.y);
	}

	inline friend Vector2& operator*=(float k, Vector2& v) {
		v.x *= k, v.y *= k;
		return v;
	}

	friend std::ostream& operator << (std::ostream& s, const Vector2& v) {
		return s << "Vec2(" << v.x << "," << v.y << ")";
	}

	inline const static Vector2& Zero() {
		const static Vector2 zeroVector = Vector2();
		return zeroVector;
	}

	// 反射函数
	inline friend Vector2 reflect(const Vector2& inDir, const Vector2& normal) {
		return inDir - 2 * (inDir * normal) * normal;
	}

	// 折射函数
	inline friend Vector2 refract(const Vector2& inDir, const Vector2& normal, float ior) {
		float cosI = Math::clamp(inDir * normal, -1, 1);
		Vector2 n = normal;
		if (cosI < 0) { cosI = -cosI; ior = 1 / ior; }
		else { n = -n; }
		float k = 1 - ior * ior * (1 - cosI * cosI);
		return k < 0 ? Vector2::Zero() : ior * inDir + (ior * cosI - sqrt(k)) * n;
	}
};


class Vector3 {
public:
	float x, y, z;
	Vector3() :x(0), y(0), z(0) {};
	Vector3(float x) :x(x), y(x), z(x) {};
	Vector3(float x, float y, float z) :x(x), y(y), z(z) {};
	Vector3(const Vector3& v) :x(v.x), y(v.y), z(v.z) {};
	~Vector3() {};

	inline float operator [] (uint8_t i) const { return (&x)[i]; }
	inline float& operator [] (uint8_t i) { return (&x)[i]; }

	inline Vector3 operator-() const {
		return Vector3(-x, -y, -z);
	}

	inline bool operator==(const Vector3& v) const {
		return x == v.x && y == v.y && z == v.z;
	}

	inline bool operator!=(const Vector3& v) const {
		return x != v.x || y != v.y || z != v.z;
	}

	inline bool isZero() const {
		return Math::isZero(x) && Math::isZero(y) && Math::isZero(z);
	}

	inline Vector3 operator+(const Vector3& v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	inline Vector3 operator-(const Vector3& v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	inline Vector3 operator+(const float v) const {
		return Vector3(x + v, y + v, z + v);
	}
	inline Vector3 operator-(const float v) const {
		return Vector3(x - v, y - v, z - v);
	}

	inline float operator*(const Vector3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	inline Vector3 operator*(const float a) const {
		return Vector3(x * a, y * a, z * a);
	}
	inline Vector3 operator/(const float a) const {
		float oneOverA = 1.0f / a;
		return Vector3(x * oneOverA, y * oneOverA, z * oneOverA);
	}

	inline Vector3& operator+=(const Vector3& v) {
		x += v.x, y += v.y, z += v.z;
		return *this;
	}
	inline Vector3& operator-=(const Vector3& v) {
		x -= v.x, y -= v.y, z -= v.z;
		return *this;
	}
	inline Vector3& operator*=(const Vector3& v) {
		x *= v.x, y *= v.y, z *= v.z;
		return *this;
	}
	inline Vector3& operator*=(const float a) {
		x *= a, y *= a, z *= a;
		return *this;
	}
	inline Vector3& operator/=(const float a) {
		float oneOverA = 1.0f / a;
		x *= oneOverA, y *= oneOverA, z *= oneOverA;
		return *this;
	}

	inline Vector3& normalize() {
		float magSq = x * x + y * y + z * z;
		if (magSq > .0f) {
			float oneOverMag = 1.0f / sqrt(magSq);
			x *= oneOverMag, y *= oneOverMag, z *= oneOverMag;
		}
		return *this;
	}

	inline Vector3 NormalizedVector() {
		float magSq = x * x + y * y + z * z;
		if (magSq > .0f) {
			float oneOverMag = 1.0f / sqrt(magSq);
			return Vector3(x * oneOverMag, y * oneOverMag, z * oneOverMag);
		}
		return Zero();
	}

	inline float length() const {
		return float(sqrt(x * x + y * y + z * z));
	}
	inline float lengthSqr() const {
		return float(x * x + y * y + z * z);
	}

	inline float dot(const Vector3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	inline friend float distance(const Vector3& v1, const Vector3& v2) {
		float dx = v1.x - v2.x;
		float dy = v1.x - v2.x;
		float dz = v1.x - v2.x;
		return sqrt(dx * dx + dy * dy + dz * dz);
	}
	inline friend float distanceSqr(const Vector3& v1, const Vector3& v2) {
		float dx = v1.x - v2.x;
		float dy = v1.x - v2.x;
		float dz = v1.x - v2.x;
		return dx * dx + dy * dy + dz * dz;
	}

	// 计算两向量的叉乘
	inline friend Vector3 cross(const Vector3& a, const Vector3& b) {
		return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}
	// 标量乘法
	inline friend Vector3 operator*(float k, const Vector3& v) {
		return Vector3(k * v.x, k * v.y, k * v.z);
	}

	inline friend Vector3& operator*=(float k, Vector3& v) {
		v.x *= k, v.y *= k, v.z *= k;
		return v;
	}

	friend std::ostream& operator << (std::ostream& s, const Vector3& v) {
		return s << "Vec3(" << v.x << "," << v.y << "," << v.z << ")";
	}

	inline const static Vector3& Zero() {
		const static Vector3 zeroVector = Vector3();
		return zeroVector;
	}

	// 反射函数
	inline friend Vector3 reflect(const Vector3& inDir, const Vector3& normal) {
		return inDir - 2 * (inDir * normal) * normal;
	}

	// 折射函数
	inline friend Vector3 refract(const Vector3& inDir, const Vector3& normal, float ior) {
		float cosI = Math::clamp(inDir * normal, -1, 1);
		Vector3 n = normal;
		if (cosI < 0) { cosI = -cosI; ior = 1 / ior; }
		else { n = -n; }
		float k = 1 - ior * ior * (1 - cosI * cosI);
		return k < 0 ? Vector3::Zero() : ior * inDir + (ior * cosI - sqrt(k)) * n;
	}
};


class Vector4 {
public:
	float x, y, z, w;
	Vector4() :x(0), y(0), z(0), w(1.f) {};
	Vector4(float x, float y, float z, float w = 1.0f) :x(x), y(y), z(z), w(w) {};
	Vector4(const Vector3& v, float w) :x(v.x), y(v.y), z(v.z), w(w) {};
	Vector4(const Vector4& v) :x(v.x), y(v.y), z(v.z), w(v.w) {};
	~Vector4() {};

	inline float operator [] (uint8_t i) const { return (&x)[i]; }
	inline float& operator [] (uint8_t i) { return (&x)[i]; }

	inline Vector4 operator-() const {
		return Vector4(-x, -y, -z);
	}

	inline bool operator==(const Vector4& v) const {
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	inline bool operator!=(const Vector4& v) const {
		return x != v.x || y != v.y || z != v.z || w != v.w;
	}

	inline Vector4 operator+(const Vector4& v) const {
		return Vector4(x + v.x, y + v.y, z + v.z);
	}
	inline Vector4 operator-(const Vector4& v) const {
		return Vector4(x - v.x, y - v.y, z - v.z);
	}

	inline float operator*(const Vector4& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	inline Vector4 operator*(const float a) const {
		return Vector4(x * a, y * a, z * a);
	}
	inline Vector4 operator/(const float a) const {
		float oneOverA = 1.0f / a;
		return Vector4(x * oneOverA, y * oneOverA, z * oneOverA);
	}

	inline Vector4& operator+=(const Vector4& v) {
		x += v.x, y += v.y, z += v.z;
		return *this;
	}
	inline Vector4& operator-=(const Vector4& v) {
		x -= v.x, y -= v.y, z -= v.z;
		return *this;
	}
	inline Vector4& operator*=(const Vector4& v) {
		x *= v.x, y *= v.y, z *= v.z;
		return *this;
	}

	inline Vector4& operator*=(const float a) {
		x *= a, y *= a, z *= a;
		return *this;
	}
	inline Vector4& operator/=(const float a) {
		float oneOverA = 1.0f / a;
		x *= oneOverA, y *= oneOverA, z *= oneOverA;
		return *this;
	}

	// 将齐次坐标转换到笛卡尔坐标
	inline explicit operator Vector3() const {
		float oneOverW = 1.0f / w;
		return Vector3(x * oneOverW, y * oneOverW, z * oneOverW);
	}

	// 标量乘法
	inline friend Vector4 operator*(float k, const Vector4& v) {
		return Vector4(k * v.x, k * v.y, k * v.z);
	}

	inline friend Vector4& operator*=(float k, Vector4& v) {
		v.x *= k, v.y *= k, v.z *= k;
		return v;
	}

	friend std::ostream& operator << (std::ostream& s, const Vector4& v) {
		return s << "Vec4(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
	}

	inline const static Vector4& Zero() {
		const static Vector4 zeroVector = Vector4();
		return zeroVector;
	}
};

namespace Vectors {
	const Vector3 one_v3 = Vector3(1.0f);
	const Vector3 zero_v3 = Vector3(0.0f);
}