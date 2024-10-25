#include "GraphicObject.h"
#include <GL/glew.h>

void GraphicObject::draw() const {
	glPushMatrix();
	glMultMatrixd(_transform.data());
	glColor3ubv(&_color.r);

	if (hasTexture()) {
		glEnable(GL_TEXTURE_2D);
		_texture.bind();
	}

	if (hasMesh()) _mesh_ptr->draw();
	
	if (hasTexture()) glDisable(GL_TEXTURE_2D);

	for (const auto& child : children()) child.draw();

	glPopMatrix();
}

BoundingBox GraphicObject::boundingBox() const {
	BoundingBox bbox = localBoundingBox();
	if(!_mesh_ptr && children().size()) bbox = children().front().boundingBox();
	for (const auto& child : children()) bbox = bbox + child.boundingBox();
	return _transform.mat() * bbox;
}

