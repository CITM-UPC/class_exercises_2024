#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"
#include "debugDraws.h"
using namespace std;

static Camera camera;

static Mesh coloredTriangle;

static Texture chessTexture;
static Mesh texturedQuad;

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(&camera.view()[0][0]);

	glTranslated(-2, 0, 0);
	coloredTriangle.draw();

	glTranslated(4, 0, 0);
	
	glEnable(GL_TEXTURE_2D);
	chessTexture.bind();
	glColor3ub(255, 255, 255);
	texturedQuad.draw();
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

static void initScene() {

	array<glm::vec3,3> triangleVertices{ glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 2, 0) };
	array<glm::u8vec3,3> triangleColors{ glm::u8vec3(255, 0, 0), glm::u8vec3(0, 255, 0), glm::u8vec3(0, 0, 255) };
	array<unsigned int,3> triangleIndices{ 0, 1, 2 };
	coloredTriangle.load(triangleVertices.data(), triangleVertices.size(), triangleIndices.data(), triangleIndices.size());
	coloredTriangle.loadColors(triangleColors.data(), triangleColors.size());

	array<glm::vec3, 4> quadVertices{ glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 2, 0), glm::vec3(-1, 2, 0)};
	array<glm::vec2, 4> quadTexCoords{ glm::vec2(0,1), glm::vec2(1,1), glm::vec2(1, 0), glm::vec2(0,0) };
	array<unsigned int, 6> quadIndices{ 0, 1, 2, 2, 3, 0 };
	texturedQuad.load(quadVertices.data(), quadVertices.size(), quadIndices.data(), quadIndices.size());
	texturedQuad.loadTexCoords(quadTexCoords.data(), quadTexCoords.size());

	vector<unsigned char> chessTextureData(64 * 64);
	for (size_t y = 0; y < 64; ++y) {
		for (size_t x = 0; x < 64; ++x) {
			chessTextureData[y * 64 + x] = ((x / 8) + (y / 8)) % 2 ? 255 : 0;
		}
	}
	auto chessTextureImage = make_shared<Image>();
	chessTextureImage->load(64, 64, 1, chessTextureData.data());
	chessTexture.setImage(chessTextureImage);
}

int main(int argc, char* argv[]) {
	// Init window and context
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("OpenGL BufferObjects and Meshes");

	// Init OpenGL
	init_opengl();

	// Init Scene
	initScene();
	
	//Init camera
	camera.zNear = 0.1;
	camera.zFar = 100.0;
	camera.transform().pos() = vec3(0, 2, 10);
	camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));

	// Set Glut callbacks
	glutDisplayFunc(display_func);
	glutIdleFunc(glutPostRedisplay);
	glutReshapeFunc(reshape_func);

	// Enter glut main loop
	glutMainLoop();

	return EXIT_SUCCESS;
}