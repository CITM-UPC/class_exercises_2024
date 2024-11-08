#include "PreFabsFactory.h"
using namespace std;

std::shared_ptr<Mesh> PreFabsFactory::MakeTriangleMesh(double size) {
	const glm::vec3 vertices[3]{ glm::vec3(-size, -size, 0), glm::vec3(size, -size, 0), glm::vec3(0, size, 0) };
	const unsigned int indices[3]{ 0, 1, 2 };
	const glm::vec2 texcoords[3]{ glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(0.5, 0) };
	const glm::vec3 normals[3]{ glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) };
	const glm::u8vec3 colors[3]{ glm::u8vec3(255, 0, 0), glm::u8vec3(0, 255, 0), glm::u8vec3(0, 0, 255) };

	auto mesh_ptr = make_shared<Mesh>();
	mesh_ptr->load(vertices, 3, indices, 3);
	mesh_ptr->loadTexCoords(texcoords,3);
	mesh_ptr->loadNormals(normals,3);
	mesh_ptr->loadColors(colors,3);
	return mesh_ptr;
}

std::shared_ptr<Mesh> PreFabsFactory::MakeQuadMesh(double size) {
	const glm::vec3 vertices[4]{ glm::vec3(-size, -size, 0), glm::vec3(size, -size, 0), glm::vec3(size, size, 0), glm::vec3(-size, size, 0) };
	const unsigned int indices[6]{ 0, 1, 2, 2, 3, 0 };
	const glm::vec2 texcoords[4]{ glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0,0) };
	const glm::vec3 normals[4]{ glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0,0,1) };
	const glm::u8vec3 colors[4]{ glm::u8vec3(255, 0, 0), glm::u8vec3(0, 255, 0), glm::u8vec3(0, 0, 255), glm::u8vec3(255,255,0) };

	auto mesh_ptr = make_shared<Mesh>();
	mesh_ptr->load(vertices, 4, indices, 6);
	mesh_ptr->loadTexCoords(texcoords,4);
	mesh_ptr->loadNormals(normals,4);
	mesh_ptr->loadColors(colors,4);
	return mesh_ptr;
}

std::shared_ptr<Image> PreFabsFactory::MakeChessTextureImage(int width, int height, int quad_size) {
	const unsigned char white = 255;
	const unsigned char black = 0;
	vector<unsigned char> colors(width * height);
	for (int y = 0; y < height; ++y) {
		int y_quad = y / quad_size;
		for (int x = 0; x < width; ++x) {
			int x_quad = x / quad_size;
			colors[y * width + x] = (x_quad + y_quad) % 2 ? white : black;
		}
	}

	const auto channels = 1;
	auto image_ptr = make_shared<Image>();
	image_ptr->load(width, height, channels, colors.data());
	return image_ptr;
}
