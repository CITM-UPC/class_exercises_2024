// Description: A simple example of how to use OpenGL with Glut in immediate mode.
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Camera.h"
#include "ImageImporter.h"
#include "Texture.h"
#include "debugDraws.h"
using namespace std;

static Camera camera;
static Texture texture;

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(&camera.view()[0][0]);

	glTranslated(-3.0, 0, 0);

	glBegin(GL_TRIANGLES);
	glColor3ub(255, 0, 0);
	glVertex3d(-1, 0, 0);
	glColor3ub(0, 255, 0);
	glVertex3d(1, 0, 0);
	glColor3ub(0, 0, 255);
	glVertex3d(0, 2, 0);
	glEnd();

	glTranslated(3.0, 0, 0);

	glLineWidth(4);
	glBegin(GL_LINE_LOOP);
	glColor3ub(255, 0, 0);
	glVertex3d(-1, 0, 0);
	glColor3ub(0, 255, 0);
	glVertex3d(1, 0, 0);
	glColor3ub(0, 0, 255);
	glVertex3d(0, 2, 0);
	glEnd();

	glTranslated(3.0, 0, 0);

	glEnable(GL_TEXTURE_2D);
	texture.bind();

	glBegin(GL_QUADS);
	glColor3ub(255, 0, 0);
	glTexCoord2d(0, 1);
	glVertex3d(-1, 0, 0);
	glColor3ub(0, 255, 0);
	glTexCoord2d(1, 1);
	glVertex3d(1, 0, 0);
	glColor3ub(0, 0, 255);
	glTexCoord2d(1, 0);
	glVertex3d(1, 2, 0);
	glColor3ub(255, 255, 0);
	glTexCoord2d(0, 0);
	glVertex3d(-1, 2, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	drawFloorGrid(100, 1);
	
	glutSwapBuffers();
}

static void init_opengl() {
	glewInit();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.5, 0.5, 0.5, 1.0);
}

static void reshape_func(int width, int height) {
	glViewport(0, 0, width, height);
	camera.aspect = static_cast<double>(width) / height;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(&camera.projection()[0][0]);
}

int main(int argc, char* argv[]) {
	// Init window and context
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("OpenGL Immediate Mode");

	// Init OpenGL
	init_opengl();

	//Init camera
	camera.zNear = 0.1;
	camera.zFar = 100.0;
	camera.transform().pos() = vec3(0, 2, 10);
	camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));

	//init texture
	auto texImage = ImageImporter::loadFromFile("Assets/Lenna.png");
	texture.setImage(texImage);

	// Set Glut callbacks
	glutDisplayFunc(display_func);
	glutIdleFunc(glutPostRedisplay);
	glutReshapeFunc(reshape_func);
	
	// Enter glut main loop
	glutMainLoop();

	return EXIT_SUCCESS;
}