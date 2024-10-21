#include "GraphicObject.h"
#include <GL/glew.h>

void GraphicObject::draw() const {
	glPushMatrix();
	glMultMatrixd(&_transform.mat()[0][0]);
	glColor3ubv(&_color.r);

	if (hasTexture()) {
		glEnable(GL_TEXTURE_2D);
		_texture.bind();
	}

	if (hasMesh()) _mesh_ptr->draw();
	
	if (hasTexture()) glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
}

