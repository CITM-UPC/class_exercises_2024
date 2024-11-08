#pragma once

#include <memory>

#include "GraphicObject.h"
#include "Mesh.h"
#include "Image.h"

class PreFabsFactory {

public:
	static std::shared_ptr<Mesh> MakeTriangleMesh(double size);
	static std::shared_ptr<Mesh> MakeQuadMesh(double size);
	static std::shared_ptr<Image> MakeChessTextureImage(int width, int height, int quad_size);
};
