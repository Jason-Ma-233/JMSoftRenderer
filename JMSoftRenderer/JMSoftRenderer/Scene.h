#pragma once

#include "Matrix.h"

struct Triangle
{
	Vector3 vertex[3];
};

class Scene {
	friend class Pipeline;
private:
	Triangle triangle;

	Matrix view;
	Matrix projection;
public:
	Scene() {}
	~Scene() {}

	void setViewMatrix(Matrix view) { this->view = view; }
	void setProjectionMatrix(Matrix projection) { this->projection = projection; }
	void setPerspective(float fov, float aspect, float zNear, float zFar) { projection.setPerspective(fov, aspect, zNear, zFar); }

	void addTriangle(float r) { this->triangle = { Vector3{r,0,0},Vector3{-r,0,0},Vector3{0,r,0} }; };

	void clear() {
		view.setIdentity();
		projection.setIdentity();
	}
};
