#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"
#include "GraphicObject.h"
using namespace std;

static Camera camera;
static GraphicObject triangle;
static GraphicObject textured_triangle;
static GraphicObject textured_quad;

static auto MakeTriangleMesh(double size) {
	const glm::vec3 vertices[] = { glm::vec3(-size, -size, 0), glm::vec3(size, -size, 0), glm::vec3(0, size, 0) };
	const unsigned int indices[] = { 0, 1, 2 };
	const glm::vec2 texcoords[] = { glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(0.5, 0) };
	const glm::vec3 normals[] = { glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) };
	const glm::u8vec3 colors[] = { glm::u8vec3(255, 0, 0), glm::u8vec3(0, 255, 0), glm::u8vec3(0, 0, 255) };

	auto mesh_ptr = make_shared<Mesh>();
	mesh_ptr->load(vertices, 3, indices, 3);
	mesh_ptr->loadTexCoords(texcoords);
	mesh_ptr->loadNormals(normals);
	mesh_ptr->loadColors(colors);
	return mesh_ptr;
}

static auto MakeQuadMesh(double size) {
	const glm::vec3 vertices[4] = { glm::vec3(-size, -size, 0), glm::vec3(size, -size, 0), glm::vec3(size, size, 0), glm::vec3(-size, size, 0)};
	const unsigned int indices[6] = { 0, 1, 2, 2, 3, 0};
	const glm::vec2 texcoords[4] = { glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0,0)};
	const glm::vec3 normals[4] = { glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0,0,1)};
	const glm::u8vec3 colors[4] = { glm::u8vec3(255, 0, 0), glm::u8vec3(0, 255, 0), glm::u8vec3(0, 0, 255), glm::u8vec3(255,255,0)};

	auto mesh_ptr = make_shared<Mesh>();
	mesh_ptr->load(vertices, 4, indices, 6);
	mesh_ptr->loadTexCoords(texcoords);
	mesh_ptr->loadNormals(normals);
	mesh_ptr->loadColors(colors);
	return mesh_ptr;
}

static auto MakeChessTextureImage(int width, int height, int quad_size) {
	const glm::u8vec3 white = glm::u8vec3(255, 255, 255);
	const glm::u8vec3 black = glm::u8vec3(0, 0, 0);

	vector<glm::u8vec3> colors(width * height);
	for (int y = 0; y < height; ++y) {
		int y_quad = y / quad_size;
		for (int x = 0; x < width; ++x) {
			int x_quad = x / quad_size;
			colors[y * width + x] = (x_quad + y_quad) % 2 ? white : black;
		}
	}

	auto image_ptr = make_shared<Image>();
	image_ptr->load(width, height, 3, colors.data());
	return image_ptr;
}

static void drawFloorGrid(int size, double step) {
	glColor3ub(0, 0, 0);
	glBegin(GL_LINES);
	for (double i = -size; i <= size; i += step) {
		glVertex3d(i, 0, -size);
		glVertex3d(i, 0, size);
		glVertex3d(-size, 0, i);
		glVertex3d(size, 0, i);
	}
	glEnd();
}

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(&camera.view()[0][0]);

	drawFloorGrid(16, 0.25);
	triangle.draw();
	textured_triangle.draw();
	textured_quad.draw();

	glutSwapBuffers();
}

static void init_opengl() {
	glewInit();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.5, 0.5, 0.5, 1.0);
}

static void reshape_func(int width, int height) {
	glViewport(0, 0, width, height);
	camera.aspect = static_cast<double>(width) / height;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(&camera.projection()[0][0]);
}

static void mouseWheel_func(int wheel, int direction, int x, int y) {
	camera.transform().translate( vec3(0, 0, direction * 0.1));
}

static void idle_func() {
	//animate triangles
	triangle.transform().rotate(0.001, vec3(0, 1, 0));
	textured_triangle.transform().rotate(0.001, vec3(1, 0, 0));
	textured_quad.transform().rotate(0.001, vec3(0, 0, 1));
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	// Iniit window and context
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Glut Simple Example");

	// Init OpenGL
	init_opengl();

	// Init camera
	camera.transform().pos() = vec3(0, 1, 4);
	camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));

	// Init triangles
	triangle.transform().pos() = vec3(0, 1, 0);
	triangle.color() = glm::u8vec3(255, 0, 0);
	textured_triangle.transform().pos() = vec3(1, 1, 0);
	textured_triangle.color() = glm::u8vec3(0, 255, 0);
	textured_quad.transform().pos() = vec3(-1, 1, 1);
	textured_quad.color() = glm::u8vec3(0, 0, 255);

	auto triangle_mesh = MakeTriangleMesh(0.5);
	auto quad_mesh = MakeQuadMesh(0.5);
	auto chess_texture_image = MakeChessTextureImage(64, 64, 8);

	triangle.setMesh(triangle_mesh);
	//triangle.setTextureImage(chess_texture_image);
	textured_triangle.setMesh(triangle_mesh);
	textured_triangle.setTextureImage(chess_texture_image);
	textured_quad.setMesh(quad_mesh);
	textured_quad.setTextureImage(chess_texture_image);


	// Set Glut callbacks
	glutDisplayFunc(display_func);
	glutIdleFunc(idle_func);
	glutReshapeFunc(reshape_func);
	glutMouseWheelFunc(mouseWheel_func);

	// Enter glut main loop
	glutMainLoop();

	return EXIT_SUCCESS;
}