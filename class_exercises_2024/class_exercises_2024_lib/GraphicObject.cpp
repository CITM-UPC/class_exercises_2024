#include "GraphicObject.h"
#include <GL/glew.h>

void GraphicObject::draw() const {
	glPushMatrix();
	glMultMatrixd(_transform.data());

	if (_material_ptr) {
		glColor4ubv(&_material_ptr->color.r);
		if (_material_ptr->texture.id()) {
			glEnable(GL_TEXTURE_2D);
			_material_ptr->texture.bind();
		}
	}

	if (hasMesh()) _mesh_ptr->draw();
	
	if (_material_ptr && _material_ptr->texture.id()) glDisable(GL_TEXTURE_2D);

	for (const auto& child : children()) child.draw();

	glPopMatrix();
}

BoundingBox GraphicObject::localBoundingBox() const {
	if (children().size()) {
		BoundingBox bbox = _mesh_ptr ? _mesh_ptr->boundingBox() : children().front().boundingBox();
		for (const auto& child : children()) bbox = bbox + child.boundingBox();
		return bbox;
	}
	else return _mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox();
}

BoundingBox GraphicObject::worldBoundingBox() const {
	BoundingBox bbox = worldTransform().mat() * (_mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox());
	for (const auto& child : children()) bbox = bbox + child.worldBoundingBox();
	return bbox;
}
