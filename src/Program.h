#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Geometry.h"
#include "InputHandler.h"
#include "RenderEngine.h"
#include "Camera.h"
#include "Catmull.h"
//#include "MeshTopology.h"


class Program {

public:

	Program();
	void start();

private:
	GLFWwindow* window;
	RenderEngine* renderEngine;

	std::vector<Geometry*> objects;

	Geometry ControlPointObj, ControlPolygonObj, SplineObj, SplineGeoObj;

	static void error(int error, const char* description);
	void setupWindow();
	void mainLoop();
};
