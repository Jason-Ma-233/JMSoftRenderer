#pragma once

#include "../Core/Matrix.h"

struct Triangle
{
	Vector3 vertex[3];
};

struct DirLight
{
	Vector3 dir;
	float intensity;
	RGBColor color;
};

class Scene {
	friend class Pipeline;

private:
	Matrix model;
	Matrix view;// 目前能用但并不准确
	Matrix projection;

	Matrix view_light, projection_light;
	DirLight dirLight;

	vector<Mesh> meshes;

public:
	Scene() {}
	~Scene() {}

	Triangle triangle[200];

	void setViewMatrix(Matrix view) { this->view = view; }
	void setProjectionMatrix(Matrix projection) { this->projection = projection; }
	void setPerspective(float fov, float aspect, float zNear, float zFar) { projection.setPerspective(fov, aspect, zNear, zFar); }
	void setOrthographic(float width, float height, float depth) { projection.scale(2.0f / width, 2.0f / height, 1.0f / depth); }
	void setLight(Matrix view, float width, float height, float depth, float intensity, RGBColor color) {
		this->view_light = view;
		projection_light.scale(2.0f / width, 2.0f / height, 1.0f / depth);
		dirLight.dir = view_light.applyDir(Vector3(0, 0, 1));
		dirLight.color = color;
		dirLight.intensity = intensity;
	}
	void setLight(Vector3 pos, float width, float height, float depth, float intensity, RGBColor color) {
		this->view_light = Matrix().setLookAt(pos, Vectors::zero_v3);
		projection_light.scale(2.0f / width, 2.0f / height, 1.0f / depth);
		dirLight.dir = pos.normalize();
		dirLight.color = color;
		dirLight.intensity = intensity;
	}


	void cameraTranslate(float y, float z) { this->view.translate(0, y, z); }
	void modelRotate(float angle) { this->model.rotate(0, 1, 0, angle); }

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
		model.setIdentity();
		view.setIdentity();
		projection.setIdentity();
		view_light.setIdentity();
		projection_light.setIdentity();
	}
};
