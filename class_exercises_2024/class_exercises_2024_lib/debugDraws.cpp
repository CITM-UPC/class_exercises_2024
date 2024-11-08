#include "debugDraws.h"
#include <GL/glew.h>

inline static void glVertex3(const vec3& v) { glVertex3dv(&v.x); }

void drawAxis(double size) {
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glColor3ub(255, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(size, 0, 0);
	glColor3ub(0, 255, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, size, 0);
	glColor3ub(0, 0, 255);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, size);
	glEnd();
}

void drawWiredQuad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3) {
	glBegin(GL_LINE_LOOP);
	glVertex3(v0);
	glVertex3(v1);
	glVertex3(v2);
	glVertex3(v3);
	glEnd();
}

void drawFloorGrid(double size, double step) {
	glColor3ub(0, 0, 0);
	glLineWidth(1);
	glBegin(GL_LINES);
	for (double i = -size; i <= size; i += step) {
		glVertex3d(i, 0, -size);
		glVertex3d(i, 0, size);
		glVertex3d(-size, 0, i);
		glVertex3d(size, 0, i);
	}
	glEnd();
}

static void drawBoundingBox(const BoundingBox& bbox) {
	glLineWidth(2.0);
	drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v011(), bbox.v010());
	drawWiredQuad(bbox.v100(), bbox.v101(), bbox.v111(), bbox.v110());
	drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v101(), bbox.v100());
	drawWiredQuad(bbox.v010(), bbox.v011(), bbox.v111(), bbox.v110());
	drawWiredQuad(bbox.v000(), bbox.v010(), bbox.v110(), bbox.v100());
	drawWiredQuad(bbox.v001(), bbox.v011(), bbox.v111(), bbox.v101());
}

void drawDebugInfoForGraphicObject(const GraphicObject& obj) {
	glPushMatrix();
	glColor3ub(255, 255, 0);
	drawBoundingBox(obj.boundingBox());
	glMultMatrixd(obj.transform().data());
	drawAxis(0.5);
	glColor3ub(0, 255, 255);
	drawBoundingBox(obj.localBoundingBox());

	glColor3ub(255, 0, 0);
	if (obj.hasMesh()) drawBoundingBox(obj.mesh().boundingBox());

	for (const auto& child : obj.children()) drawDebugInfoForGraphicObject(child);
	glPopMatrix();
}
/*
void drawSegment(const Segment& s) {
	glLineWidth(4.0);
	glBegin(GL_LINES);
	glColor3ub(0, 0, 0);
	glVertex3(s.start);
	glColor3ub(255, 255, 255);
	glVertex3(s.end);
	glEnd();
}

void drawIntersectionPoint(const vec3& point) {
	glDisable(GL_DEPTH_TEST);
	glPointSize(8);
	glBegin(GL_POINTS);
	glColor3ub(255, 0, 0);
	glVertex3(point);
	glEnd();
	glEnable(GL_DEPTH_TEST);
}
*/