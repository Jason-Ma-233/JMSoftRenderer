#include "Window.h"
#include "Pipeline.h"
#include "ShaderPrefab.h"

using namespace std;

const Pipeline::RenderState states[] = { 
	Pipeline::WIREFRAME, 
	Pipeline::COLOR, 
	Pipeline::TEXTURE,
	Pipeline::COLOR_TEXTURE, 
	Pipeline::SHADING
};
const ShadeFunc shaders[] = {
	FragmentShader::depth(1.5f, 0),
	FragmentShader::normal(),
	FragmentShader::lambert_direction_light(Vector3(.5f, .5f, -1)),
	FragmentShader::phong_direction_light(Vector3(1, 1, -1), Colors::Black, Colors::White * .3f, Colors::White * .9f, 7.f),
	FragmentShader::blinn_phong_direction_light(Vector3(1, 1, -1), Colors::Black, Colors::White * .3f, Colors::White * .9f, 7.f),
	FragmentShader::blinn_phong_direction_light_color_textured(Vector3(1, 1, -1), Colors::White * .1f, Colors::White * .45f, Colors::White * 1.5f, 6.f),
};
const int shaderNum = 6;
const int sceneNum = 4;

static float aspect;
static float translateZ = 1.5f;
static float rotateX = 0, rotateY = 0;
static shared_ptr<IntBuffer> texture;
static ShadeFunc currentShader;

shared_ptr<Mesh> createSphere(float radius, int space = 10, const shared_ptr<IntBuffer> & texture = nullptr, const ShadeFunc & shader = nullptr) {
	const float & dtr = Math::DEGREE_TO_RADIUS;
	size_t vertexCount = (180 / space) * (360 / space) * 4;
	shared_ptr<Mesh> m = make_shared<Mesh>();
	Vertex v;
	v.color = Colors::White;
	for (int b = 0; b < 180; b += space) {
		for (int a = 0; a < 360; a += space) {
			v.point.x = radius * sin(a * dtr) * sin(b * dtr);
			v.point.y = radius * cos(a * dtr) * sin(b * dtr);
			v.point.z = radius * cos(b * dtr);
			v.texCoord.x = a / 360.f;
			v.texCoord.y = b / 180.f;
			v.normal = v.point;
			m->vertices.push_back(v);

			v.point.x = radius * sin(a * dtr) * sin((b + space) * dtr);
			v.point.y = radius * cos(a * dtr) * sin((b + space) * dtr);
			v.point.z = radius * cos((b + space) * dtr);
			v.texCoord.x = a / 360.f;
			v.texCoord.y = (b + space) / 180.f;
			v.normal = v.point;
			m->vertices.push_back(v);

			v.point.x = radius * sin((a + space) * dtr) * sin(b * dtr);
			v.point.y = radius * cos((a + space) * dtr) * sin(b * dtr);
			v.point.z = radius * cos(b * dtr);
			v.texCoord.x = (a + space) / 360.f;
			v.texCoord.y = b / 180.f;
			v.normal = v.point;
			m->vertices.push_back(v);

			v.point.x = radius * sin((a + space) * dtr) * sin((b + space) * dtr);
			v.point.y = radius * cos((a + space) * dtr) * sin((b + space) * dtr);
			v.point.z = radius * cos((b + space) * dtr);
			v.texCoord.x = (a + space) / 360.f;
			v.texCoord.y = (b + space) / 180.f;
			v.normal = v.point;
			m->vertices.push_back(v);
		}
	}
	
	for (size_t i = 0; i < vertexCount - 2; i++) {
		// 保证每个面方向一致
		m->primitives.push_back(i % 2 ? Primitive{ i, i + 1, i + 2 } : Primitive{ i + 2, i + 1, i });
	}

	m->texture = texture;
	m->shadeFunc = shader;
	return m;
}

void solarSystem(Scene & scene) {
	static shared_ptr<Mesh> sun = createSphere(3, 9, nullptr,
		[](RGBColor & out, const Vector3 & pos, const RGBColor & color, const Vector3 & normal, const shared_ptr<IntBuffer> & texture, const TexCoord & texCoord) -> bool {
		out = RGBColor(1, 1, 0);
		return true;
	});
	static shared_ptr<Mesh> earth = createSphere(1, 10);
	static shared_ptr<Mesh> moon = createSphere(0.5, 15);

	scene.addMesh(sun);

	static float earthRevolution, earthRotation;
	earthRevolution += 0.1f;
	earthRotation += 1.f;

	Vector3 lightVec(0, 0, 1);
	ShadeFunc shader = FragmentShader::blinn_phong_direction_light(lightVec, Colors::White * .1f, Colors::White * .45f, Colors::White * 1.5f, 4.f);

	scene.rotate(0, 1, 0, earthRotation);
	scene.translate(0, 0, 10);
	scene.rotate(0, 1, 0, earthRevolution);
	earth->shadeFunc = shader;
	scene.addMesh(earth);

	static float moonRevolution;
	moonRevolution += 0.24f;
	scene.rotate(0, 1, 0, moonRevolution);
	scene.translate(0, 0, 3);
	
	moon->shadeFunc = shader;
	scene.addMesh(moon);
}

void createScene(Scene & scene, int index) {
	scene.clear();
	shared_ptr<Mesh> m = make_shared<Mesh>();
	m->texture = texture;
	m->shadeFunc = currentShader;
	switch (index) {
	case 0:
		m->vertices = { 
			{ Vector3(0,1,0), Colors::Green, TexCoord{ 0, 1 } },
			{ Vector3(1,0,0), Colors::Blue, TexCoord{ 1, 0 } },
			{ Vector3(0,0,0), Colors::Red, TexCoord{ 0, 0 } },
		};
		m->primitives = { Primitive{ 0,1,2, Vector3(0,0,1) } };
		scene.addMesh(m, Matrix44().translate(-.5f, -.5f, 0));
		break;
	case 1:
		m->vertices = {
			{ Vector3(-0.5f, -0.5f,  0.5f), Colors::Red,   TexCoord{ 0, 0 } },
			{ Vector3(0.5f, -0.5f,  0.5f),  Colors::Green, TexCoord{ 1, 0 } },
			{ Vector3(0.5f,  0.5f,  0.5f),  Colors::Blue,  TexCoord{ 1, 1 } },
			{ Vector3(-0.5f,  0.5f,  0.5f), Colors::White, TexCoord{ 0, 1 } },
			{ Vector3(-0.5f, -0.5f, -0.5f), Colors::Blue,  TexCoord{ 1, 0 } },
			{ Vector3(-0.5f,  0.5f, -0.5f), Colors::Red,   TexCoord{ 1, 1 } },
			{ Vector3(0.5f,  0.5f, -0.5f),  Colors::Green, TexCoord{ 0, 1 } },
			{ Vector3(0.5f, -0.5f, -0.5f),  Colors::White, TexCoord{ 0, 0 } }
		};
		m->primitives = {
			Primitive{ 0,1,2, Vector3(0,0,1) }, Primitive{ 0,2,3, Vector3(0,0,1) },
			Primitive{ 4,5,6, Vector3(0,0,-1) }, Primitive{ 4,6,7, Vector3(0,0,-1) },
			Primitive{ 5,3,2, Vector3(0,1,0) }, Primitive{ 5,2,6, Vector3(0,1,0) },
			Primitive{ 4,7,1, Vector3(0,-1,0) }, Primitive{ 4,1,0, Vector3(0,-1,0) },
			Primitive{ 7,6,2, Vector3(1,0,0) }, Primitive{ 7,2,1, Vector3(1,0,0) },
			Primitive{ 4,0,3, Vector3(-1,0,0) }, Primitive{ 4,3,5, Vector3(-1,0,0) },
		};
		scene.addMesh(m);
		break;
	case 2:
		for (float i = 0; i < 360; i += 5) {
			scene.addLine(Line{
			    Vertex { Vector3(0,0,0), Colors::White },
			    Vertex { Vector3(0.7f * cos(i),0.7f * sin(i),1), Colors::Random() }
			});
		}
		break;
	case 3:
		solarSystem(scene);
		break;
	}
}



int main() {
	SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);

	texture = CreateTexture("C:\\Users\\13121\\Desktop\\tex.png");

	IntBuffer image(1280, 720);
	Pipeline pipeline(image);

	Scene scene;
	Window window(image.getWidth(), image.getHeight(), _T("SoftRenderer"));
	aspect = image.aspect();

	bool kbhit[3] = { false };
	int sceneI = 1, modeI = 0, shaderI = 0;
	currentShader = shaders[shaderI];

	createScene(scene, sceneI);
	
	while (window.is_run()) {
		scene.setPerspective(70, aspect, 0.5f, 1000);
		scene.setViewMatrix(Matrix44().rotate(0, 1, 0, rotateY).rotate(1, 0, 0, rotateX).translate(0, 0, translateZ));

		pipeline.render(scene);

		memcpy(window(), image(), image.getSize() * sizeof(int));
		window.update();
		ostringstream s;
		s << "SoftRenderer(Space switch scene, Ctrl switch mode, Shift switch shader) Fps:" << window.get_fps();
		window.setTitle(_T(s.str().c_str()));
		if (window.is_key(VK_ESCAPE)) window.destory();
		if (window.is_key(VK_LEFT)) rotateY -= 2.5f;
		if (window.is_key(VK_RIGHT)) rotateY += 2.5f;
		if (window.is_key(VK_DOWN)) rotateX -= 2.5f;
		if (window.is_key(VK_UP)) rotateX += 2.5f;
		if (window.is_key('W')) translateZ -= .05f;
		if (window.is_key('S')) translateZ += .05f;

		if (window.is_key(VK_CONTROL)) {
			if (!kbhit[0]) {
				modeI = ++modeI % 5;
				pipeline.setRenderState(Pipeline::RenderState(states[modeI]));
			}
			kbhit[0] = true;
		} else kbhit[0] = false;
		if (window.is_key(VK_SPACE)) {
			if (!kbhit[1]) {
				sceneI = ++sceneI % sceneNum;
				createScene(scene, sceneI);
			}
			kbhit[1] = true;
		} else {
			kbhit[1] = false;
			if (sceneI == 3) createScene(scene, 3);
		}
		if (window.is_key(VK_SHIFT)) {
			if (!kbhit[2]) {
				shaderI = ++shaderI % shaderNum;
				currentShader = shaders[shaderI];
				createScene(scene, sceneI);
			}
			kbhit[2] = true;
		} else kbhit[2] = false;
		Sleep(1);
	}
}