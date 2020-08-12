#include "header/FrameBuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "include\stb_image.h"

shared_ptr<IntBuffer> CreateTexture(const char* filename) {
	int width, height, comp;
	stbi_uc* data = stbi_load(filename, &width, &height, &comp, STBI_rgb);
	if (!data) return shared_ptr<IntBuffer>();
	shared_ptr<IntBuffer> buffer = make_shared<IntBuffer>(width, height);
	for (int i = 0; i < width * height; i++) {
		// Ê¹ÓÃimageÌî³ätexture
		*(*buffer)(i) = (data[3 * i] << 16) | (data[3 * i + 1] << 8) | data[3 * i + 2];
	}
	stbi_image_free(data);
	return buffer;
}