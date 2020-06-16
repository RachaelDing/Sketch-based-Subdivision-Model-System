#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "RenderEngine.h"
extern std::vector<glm::vec3> control_points;
extern int flg_point_selected;
extern int order_k;
extern float increment_u;
extern float u_pos;
extern int flg_G_mode;
extern std::vector<float> weights;
extern int flg_NURBS_mode;
double pointDistance(glm::vec3 a, glm::vec3 b);
extern int stage;
extern int face_flg;

class InputHandler {

public:
	static void setUp(RenderEngine* renderEngine);

	static void key(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse(GLFWwindow* window, int button, int action, int mods);
	static void motion(GLFWwindow* window, double x, double y);
	static void scroll(GLFWwindow* window, double x, double y);
	static void reshape(GLFWwindow* window, int width, int height);

private:
	static RenderEngine* renderEngine;

	static int mouseOldX;
	static int mouseOldY;
};
