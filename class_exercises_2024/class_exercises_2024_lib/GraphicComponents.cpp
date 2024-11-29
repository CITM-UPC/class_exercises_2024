#include "GraphicComponents.h"
#include <GL/glew.h>

void MeshComponent::draw() const {
	if (_material) {
		glColor4ubv(&_material->color.r);
		if (_material->texture.id()) {
			glEnable(GL_TEXTURE_2D);
			_material->texture.bind();
		}
	}

	if (_mesh) _mesh->draw();

	if (_material && _material->texture.id()) glDisable(GL_TEXTURE_2D);
}
