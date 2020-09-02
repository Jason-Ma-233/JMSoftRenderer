#include "header/Window.h"
#include "header/Pipeline.h"
#include "header/OBJ_Loader.h"

using namespace std;

void addMesh(Scene& scene, vector<objl::Mesh> objMeshes, shared_ptr<IntBuffer> texture, RGBColor color = Colors::White) {
	for (auto& objMesh : objMeshes)
	{
		Mesh mesh;
		for (size_t i = 0; i < objMesh.Vertices.size(); i++)
		{
			auto objVert = objMesh.Vertices[i];
			Vertex v;
			v.point = Vector3(objVert.Position.X, objVert.Position.Y, objVert.Position.Z);
			v.normal = Vector3(objVert.Normal.X, objVert.Normal.Y, objVert.Normal.Z);
			v.texCoord = Vector2(objVert.TextureCoordinate.X, objVert.TextureCoordinate.Y);
			v.color = RGBColor(objVert.TextureCoordinate.X, objVert.TextureCoordinate.Y, 1.0f);
			mesh.vertices.push_back(v);
		}
		mesh.indices = objMesh.Indices;
		mesh.texture = texture;
		mesh.color = color;
		scene.addMesh(mesh);
	}
}


int	main(void) {

	IntBuffer colorBuffer(1280, 720);
	Pipeline pipeline(colorBuffer, 512, ProjectionMethod::Perspective, false);
	Window window(colorBuffer.get_width(), colorBuffer.get_height(), _T("JM Soft Renderer  "));

	// Load .obj File
	objl::Loader loader;
	//const char* texture_path = "../../../../models/spot/uv_texture.png";
	const char* texture_path = "../../../../models/spot/spot_texture.png";
	//const char* obj_path = "../../../../models/spot/triangle.obj";
	//const char* obj_path = "../../../../models/spot/_spot_triangulated_good.obj";
	const char* obj_path = "../../../../models/spot/sphere.obj";
	auto tex = CreateTexture(texture_path);
	if (!loader.LoadFile(obj_path)) {
		cout << "File loading failed!" << endl;
		return 1;
	}

	Scene scene;
	scene.setLight(
		Vector3(1.0f, 1.0f, -1.0f),
		4.0f, 4.0f, 10.0f,
		//Matrix().rotate(0, 1, 0, 70.0f).rotate(1, 0, 0, -60.0f).translate(-0.2f, 0.3f, 3.5f),
		//4.0f, 4.0f, 10.0f,
		2.0f, RGBColor(0.98f, 0.92f, 0.89f)
	);

	scene.setViewMatrix(Matrix().translate(0, 0, 2.5f));
	scene.setPerspective(60.0f, colorBuffer.get_aspect(), 0.1f, 10.0f);


	addMesh(scene, loader.LoadedMeshes, tex);
	//addMesh(scene, loader.LoadedMeshes, NULL, RGBColor(0.5f));


	while (window.is_run())
	{
		pipeline.clearBuffers(Colors::Black);

		if (pipeline.enableShadow)
			pipeline.renderShadowMap(scene);
		pipeline.renderMeshes(scene);

		memcpy(window(), colorBuffer(), colorBuffer.get_size() * sizeof(int));
		window.title = (std::ostringstream() << "Roughness:" << pipeline.roughness << "  Metallic:" << pipeline.metallic).str();
		window.update();

		// input
		if (window.is_key(VK_ESCAPE)) window.destory();
		if (window.is_key('W')) scene.cameraTranslate(0.0f, -0.02f);
		if (window.is_key('S')) scene.cameraTranslate(0.0f, 0.02f);
		if (window.is_key('E')) scene.cameraTranslate(-0.02f, 0.0f);
		if (window.is_key('Q')) scene.cameraTranslate(0.02f, 0.0f);

		if (window.is_key('A')) scene.modelRotate(2.0f);
		else if (window.is_key('D')) scene.modelRotate(-2.0f);
		else scene.modelRotate(0.25f);

		if (window.is_key(VK_LEFT)) pipeline.roughness -= 0.01f;
		if (window.is_key(VK_RIGHT)) pipeline.roughness += 0.01f;
		if (window.is_key(VK_UP)) pipeline.metallic += 0.01f;
		if (window.is_key(VK_DOWN)) pipeline.metallic -= 0.01f;
		pipeline.roughness = Math::clamp(pipeline.roughness);
		pipeline.metallic = Math::clamp(pipeline.metallic);
	}


	return 0;
}