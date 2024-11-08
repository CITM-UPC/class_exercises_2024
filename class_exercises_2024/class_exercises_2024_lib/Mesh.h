#pragma once

#include <vector>
#include <span>
#include <ranges>
#include <glm/glm.hpp>
#include "BufferObject.h"
#include "BoundingBox.h"

class Mesh {

	BufferObject _texCoordBuffer;
	BufferObject _normalBuffer;
	BufferObject _colorBuffer;
	BufferObject _vertexBuffer;
	BufferObject _indexBuffer;

	std::vector<glm::vec3> _vertices;
	std::vector<unsigned int> _indices;

	BoundingBox _boundingBox;

public:
	struct Face { unsigned int v0, v1, v2; };

	struct Triangle {
		const glm::vec3& v0;
		const glm::vec3& v1;
		const glm::vec3& v2;
	};

	const auto& vertices() const { return _vertices; }
	const auto& indices() const { return _indices; }
	const auto faces() const { return std::span((Face*)_indices.data(), _indices.size()/3); }
	const auto triangles() const { return faces() | std::views::transform([this](const auto& face){return Triangle{ _vertices[face.v0],_vertices[face.v1] ,_vertices[face.v2] }; }); }
	const auto& boundingBox() const { return _boundingBox; }

	void load(const glm::vec3* verts, size_t num_verts, const unsigned int* indexs, size_t num_indexs);
	void loadTexCoords(const glm::vec2* texCoords, size_t num_verts);
	void loadNormals(const glm::vec3* normals, size_t num_verts);
	void loadColors(const glm::u8vec3* colors, size_t num_verts);
	void draw() const;

};