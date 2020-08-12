#pragma once

#include "../Core/Matrix.h"


struct Triangle
{
	Vector3 vertex[3];
};

class Scene {
	friend class Pipeline;

private:
	Matrix view;
	Matrix projection;

	vector<Mesh> meshes;

public:
	Scene() {}
	~Scene() {}

	Triangle triangle[200];

	void setViewMatrix(Matrix view) { this->view = view; }
	void setProjectionMatrix(Matrix projection) { this->projection = projection; }
	void setPerspective(float fov, float aspect, float zNear, float zFar) { projection.setPerspective(fov, aspect, zNear, zFar); }

	void addMesh(Mesh mesh) {
		meshes.push_back(mesh);
	}

	void addTriangle(float offset = -0.1f) {
		for (size_t x = 0; x < 20; x++)
		{
			for (size_t y = 0; y < 10; y++)
			{
				float u = x * 0.2f, v = y * 0.2f;

				this->triangle[x * 10 + y] =
				{
					Vector3{-1.8f + offset + u,-1.0f + v,2},
					Vector3{-2.0f + u,-1.0f + v,2},
					Vector3{-2.0f + u,-0.8f + offset + v,2}
				};

			}
		}
	};

	void clear() {
		view.setIdentity();
		projection.setIdentity();
	}
};
