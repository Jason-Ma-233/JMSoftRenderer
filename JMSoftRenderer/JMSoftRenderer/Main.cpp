#include "Window.h"
#include "Pipeline.h"
#include "OBJ_Loader.h"


using namespace std;



void addMesh(Scene& scene, vector<objl::Mesh> objMeshes) {
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

		scene.addMesh(mesh);
	}
}


int	main(void) {

	IntBuffer colorBuffer(1280, 720);
	Pipeline pipeline(colorBuffer);
	Window window(colorBuffer.get_width(), colorBuffer.get_height(), _T("JM Soft Renderer  "));

	// Load .obj File
	objl::Loader loader;
	const char* obj_path = "D:\\Code\\Git\\JMSoftRenderer\\JMSoftRenderer\\models\\spot\\_spot_triangulated_good.obj";
	bool loadout = loader.LoadFile(obj_path);

	Scene scene;
	//scene.addTriangle();
	addMesh(scene, loader.LoadedMeshes);


	while (window.is_run())
	{
		pipeline.clearBuffers(Colors::Black);
		scene.clear();
		scene.setPerspective(60.0f, colorBuffer.get_aspect(), 0.1f, 1000.0f);

		pipeline.renderMeshes(scene);


		memcpy(window(), colorBuffer(), colorBuffer.get_size() * sizeof(int));
		window.update();
	}


	return 0;
}