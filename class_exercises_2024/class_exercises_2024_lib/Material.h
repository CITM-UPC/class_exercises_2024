#pragma once

#include "types.h"
#include "Texture.h"

struct Material {
	color4 color = Colors::White;
	Texture texture;
};
