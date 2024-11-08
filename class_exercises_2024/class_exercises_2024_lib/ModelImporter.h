#pragma once

#include <string>
#include "GraphicObject.h"

class ModelImporter {
public:
	static GraphicObject loadFromFile(const std::string& path);
};

