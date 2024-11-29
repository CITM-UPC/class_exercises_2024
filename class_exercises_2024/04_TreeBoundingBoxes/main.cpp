#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"
#include "GraphicObject.h"
#include "ModelImporter.h"
#include "PreFabsFactory.h"
#include "intersect.h"
#include "debugDraws.h"
using namespace std;

using hrclock = chrono::high_resolution_clock;
static const auto FPS = 60;
static const auto FRAME_TIME = 1.0s / FPS;

static bool paused = true;
static Camera camera;
static GraphicObject scene;

static Segment mouse_ray;
static vec3 intersect_point;

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(&camera.view()[0][0]);

	scene.draw();

	//draw debug
	drawFloorGrid(scene.boundingBox().size().x * 2, 1);
	drawSegment(mouse_ray);
	drawIntersectionPoint(intersect_point);
	drawDebugInfoForGraphicObject(scene);

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

static void mouseWheel_func(int wheel, int direction, int x, int y) {
	camera.transform().translate(vec3(direction * scene.boundingBox().size().z * 0.025, 0, direction * scene.boundingBox().size().z * 0.025));
}

static void idle_func() {
	static auto last_time = hrclock::now();
	const auto current_time = hrclock::now();
	const auto delta_time = current_time - last_time;
	if (delta_time >= FRAME_TIME) {
		if (!paused) {
			//rotate everything 360 degrees per 10 seconds
			const auto angle_vel = glm::radians(360.0) / 10.0;
			scene.children().front().transform().rotate(angle_vel * FRAME_TIME.count(), vec3(0, 0, 1));
			scene.children().front().children().front().transform().rotate(angle_vel * FRAME_TIME.count(), vec3(0, 0, 1));
			scene.children().front().children().front().children().front().transform().rotate(angle_vel * FRAME_TIME.count(), vec3(0, 0, 1));
		}

		static auto last_mouse_ray = mouse_ray;
		if (last_mouse_ray != mouse_ray) {
			const auto t0 = hrclock::now();
			double intersect_distance = numeric_limits<double>::max();
			if (intersectRayWithGraphicObject(mouse_ray, scene, intersect_distance)) intersect_point = mouse_ray.start + mouse_ray.dir() * intersect_distance;
			const auto t1 = hrclock::now();
			const chrono::duration<double, std::milli> dt(t1 - t0);
			last_mouse_ray = mouse_ray;
		}

		glutPostRedisplay();
		last_time = current_time;
	}
}

static void initScene() {

	const auto triangle_mesh = PreFabsFactory::MakeTriangleMesh(0.5);
	const auto quad_mesh = PreFabsFactory::MakeQuadMesh(0.5);
	const auto chess_texture_image = PreFabsFactory::MakeChessTextureImage(64, 64, 8);

	auto& triangle = scene.emplaceChild();
	triangle.transform().pos() = vec3(0, 0, 0);
	triangle.setMesh(triangle_mesh);
	triangle.setDefaultMaterial();
	triangle.material().color = Colors::Red;

	auto& child_textured_triangle = triangle.emplaceChild();
	child_textured_triangle.transform().pos() = vec3(2, 0, 0);
	child_textured_triangle.setMesh(triangle_mesh);
	child_textured_triangle.setDefaultMaterial();
	child_textured_triangle.material().color = Colors::Green;
	child_textured_triangle.material().texture.setImage(chess_texture_image);

	auto& child_textured_quad = child_textured_triangle.emplaceChild();
	child_textured_quad.transform().pos() = vec3(2, 0, 0);
	child_textured_quad.setMesh(quad_mesh);
	child_textured_quad.setDefaultMaterial();
	child_textured_quad.material().color = Colors::Blue;
	child_textured_quad.material().texture.setImage(chess_texture_image);
}

static auto worldToNDC(const mat4& proj, const mat4& view, const vec3& world_pos) {
	auto ndc_pos = proj * view * vec4(world_pos, 1.0);
	ndc_pos /= ndc_pos.w;
	return vec3(ndc_pos);
}

static auto NDCToWorld(const mat4& proj, const mat4& view, const vec3& ndc_pos) {
	auto world_pos = glm::inverse(proj * view) * vec4(ndc_pos, 1.0);
	world_pos /= world_pos.w;
	return vec3(world_pos);
}

int main(int argc, char* argv[]) {
	// Init window and context
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Tree Transform");

	// Init OpenGL
	init_opengl();

	// Init Scene from file
	scene = ModelImporter::loadFromFile("Assets/street2.fbx");

	//Init camera relative to the scene
	const auto scene_bbox = scene.boundingBox();
	camera.zFar = scene_bbox.size().z * 4.0;
	camera.transform().pos() = scene_bbox.center() + vec3(0, 0, scene_bbox.size().z);
	camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));

	// Set Glut callbacks
	glutDisplayFunc(display_func);
	glutIdleFunc(idle_func);
	glutReshapeFunc(reshape_func);
	glutMouseWheelFunc(mouseWheel_func);
	glutKeyboardFunc([](unsigned char key, int x, int y) { paused = !paused; });
	glutMouseFunc([](int button, int state, int x, int y) {
		if (button != GLUT_LEFT_BUTTON) return;
		if (state != GLUT_DOWN) return;
		double normalized_x = 2.0 * x / glutGet(GLUT_WINDOW_WIDTH) - 1.0;
		double normalized_y = 1.0 - 2.0 * y / glutGet(GLUT_WINDOW_HEIGHT);
		const vec3 ndc_near_pos = vec3(normalized_x, normalized_y, -1);
		const vec3 ndc_far_pos = vec3(normalized_x, normalized_y, 1);
		mouse_ray.start = NDCToWorld(camera.projection(), camera.view(), ndc_near_pos);
		mouse_ray.end = NDCToWorld(camera.projection(), camera.view(), ndc_far_pos);
		});

	// Enter glut main loop
	glutMainLoop();

	return EXIT_SUCCESS;
}