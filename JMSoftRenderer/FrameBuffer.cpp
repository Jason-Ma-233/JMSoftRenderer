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

shared_ptr<IntBuffer> DownSample(shared_ptr<IntBuffer>& buffer) {
	shared_ptr<IntBuffer> newBuffer = make_shared<IntBuffer>(buffer->get_width() * 0.5f, buffer->get_height() * 0.5f);
	for (size_t x = 0; x < newBuffer->get_width(); x++)
	{
		for (size_t y = 0; y < newBuffer->get_height(); y++) {
			RGBColor x1 = RGBColor(buffer->tex2DScreenSpace(x * 2, y * 2)).gammaCorrect_inv(2.2);
			RGBColor x2 = RGBColor(buffer->tex2DScreenSpace(x * 2 + 1, y * 2)).gammaCorrect_inv(2.2);
			RGBColor y1 = RGBColor(buffer->tex2DScreenSpace(x * 2, y * 2 + 1)).gammaCorrect_inv(2.2);
			RGBColor y2 = RGBColor(buffer->tex2DScreenSpace(x * 2 + 1, y * 2 + 1)).gammaCorrect_inv(2.2);
			newBuffer->set(x, y, (x1 * 0.25f + x2 * 0.25f + y1 * 0.25f + y2 * 0.25f).gammaCorrect(2.2).toRGBInt());
		}
	}
	return newBuffer;
}

